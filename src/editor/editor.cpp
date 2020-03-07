#include <fstream>

#include "editor.hpp"
#include "system.hpp"

Editor::Editor(Ivec pos, Ivec size, short border_size) :
Instance(pos, size, border_size)
{
	font = Resources::get_font(Resources::MONO);
	this->render_texture = nullptr;
	this->changed = true;
	this->row = 0;
	this->col = 0;
	SDL_StartTextInput();
	scroll_chars = { 0, 0 };

	read("res/test/test.txt");
}

Editor::~Editor()
{
}

void Editor::update()
{
	while((int)row - scroll_chars.x >= num_cells.x - 1)
	{
		scroll_chars.x += 2;
		changed = true;
	}
	while((int)row - scroll_chars.x < 0)
	{
		scroll_chars.x -= 10;
		if(scroll_chars.x < 0)
		{
			scroll_chars.x = 0;
			break;
		}
		changed = true;
	}
	while((int)col - scroll_chars.y >= num_cells.y - 1)
	{
		scroll_chars.y += 2;
		changed = true;
	}
	while((int)col - scroll_chars.y < 0)
	{
		scroll_chars.y -= 10;
		if(scroll_chars.y < 0)
		{
			scroll_chars.y = 0;
			break;
		}
		changed = true;
	}
}

void Editor::render()
{
	Instance::render();
	// rendering cursor

	TTF_SizeText(font, " ", &glyph_size.x, &glyph_size.y);
	if(changed && text.size() > 0)
	{
		this->num_cells =
			Ivec(render_size.x / glyph_size.x, render_size.y / glyph_size.y);
		if(render_texture != nullptr)
		{
			SDL_DestroyTexture(render_texture);
		}
		// a new blank texture
		SDL_Texture* render_complete = SDL_CreateTexture(
			SDL::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			render_size.x - border_size * 2, render_size.y - border_size * 2);

		// setting to draw to blank texture
		SDL_SetRenderTarget(SDL::renderer, render_complete);
		SDL_Surface* surface;
		SDL_Texture* texture_part;

		// rendering each layer to blank texture
		for(size_t i = 0; i < text.size(); i++)
		{
			const char* surface_text = text[i].c_str();
			surface = TTF_RenderText_Blended(font, surface_text,
											 SDL_Color{ 255, 0, 0, 255 });
			texture_part = SDL_CreateTextureFromSurface(SDL::renderer, surface);
			SDL_Rect line_pos;
			SDL_QueryTexture(texture_part, nullptr, nullptr, &line_pos.w,
							 &line_pos.h);
			line_pos.x = -scroll_chars.x * glyph_size.x;
			line_pos.y = i * line_pos.h - scroll_chars.y * glyph_size.y;
			SDL_RenderCopy(SDL::renderer, texture_part, NULL, &line_pos);
			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture_part);
		}
		// assigning blank texture to classes texture and resetting renderer
		render_texture = render_complete;
		SDL_SetRenderTarget(SDL::renderer, NULL);
		changed = false;
	}
	if(render_texture != nullptr)
	{
		// rendering all text
		SDL_Rect rect = { pos.x + border_size, pos.y + border_size };
		SDL_QueryTexture(render_texture, nullptr, nullptr, &rect.w, &rect.h);
		SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &rect);
	}

	SDL_Rect cursor = { pos.x + glyph_size.x * (int)row -
							scroll_chars.x * glyph_size.x + border_size,
						pos.y + glyph_size.y * (int)col -
							scroll_chars.y * glyph_size.y + border_size,
						glyph_size.x, glyph_size.y };

	SDL_SetRenderDrawColor(SDL::renderer, 255, 255, 255, 100);
	SDL_SetRenderDrawBlendMode(SDL::renderer, SDL_BLENDMODE_BLEND);
	if(active)
	{
		SDL_RenderFillRect(SDL::renderer, &cursor);
	}
	else
	{
		SDL_RenderDrawRect(SDL::renderer, &cursor);
	}
}

void Editor::process_event(const SDL_Event& event)
{
	switch(event.type)
	{
		case SDL_TEXTINPUT:
		{
			// normal text
			for(char* c = (char*)event.text.text; *c != '\0'; c++)
			{
				this->text[col].insert(this->text[col].begin() + row, *c);
				row++;
			}
			changed = true;
		}
		break;
		case SDL_KEYDOWN:
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_BACKSPACE:
				{
					if(this->text[col].size() > 0 && row != 0)
					{
						// removal of single char
						row--;
						this->text[col].erase(this->text[col].begin() + row);
					}
					else if(col != 0)
					{
						// removal of newline char
						// new line gets moved to end of prev line
						this->col--;
						if(text[col + 1].size() != 0)
						{
							std::string copy = text[col + 1];
							text.erase(text.begin() + col + 1);
							text[col].append(copy);
							row = text[col].size() - copy.size();
						}
					}
					changed = true;
				}
				break;
				case SDLK_RETURN:
				{
					const Uint8* keys = SDL_GetKeyboardState(NULL);
					if(keys[SDL_SCANCODE_LSHIFT])
					{
						this->text.emplace(this->text.begin() + col);
						changed = true;
						row = 0;
					}
					else
					{
						// inserts new line
						std::string copy_to_end = this->text[col].substr(row);
						this->text[col].erase(this->text[col].begin() + row,
											  this->text[col].end());
						col++;
						row = 0;
						changed = true;
						this->text.emplace(this->text.begin() + col);
						this->text[col].append(copy_to_end);
					}
				}
				break;
				case SDLK_k:
				{
					const Uint8* keys = SDL_GetKeyboardState(NULL);
					if(keys[SDL_SCANCODE_LCTRL] && !keys[SDL_SCANCODE_LSHIFT])
					{
						// moves up one char
						if(col != 0)
						{
							col--;
							if(row > this->text[col].size())
							{
								row = this->text[col].size();
							}
						}
					}
				}
				break;
				case SDLK_UP:
				{
					// moves up one char
					if(col != 0)
					{
						col--;
						if(row > this->text[col].size())
						{
							row = this->text[col].size();
						}
					}
				}
				break;
				case SDLK_j:
				{
					const Uint8* keys = SDL_GetKeyboardState(NULL);
					if(keys[SDL_SCANCODE_LCTRL] && !keys[SDL_SCANCODE_LSHIFT])
					{
						// moves down one char
						col++;
						if(col > this->text.size() - 1)
						{
							col = this->text.size() - 1;
						}
						if(row > this->text[col].size())
						{
							row = this->text[col].size();
						}
					}
				}
				break;
				case SDLK_DOWN:
				{
					// moves down one char
					col++;
					if(col > this->text.size() - 1)
					{
						col = this->text.size() - 1;
					}
					if(row > this->text[col].size())
					{
						row = this->text[col].size();
					}
				}
				break;
				case SDLK_l:
				{
					const Uint8* keys = SDL_GetKeyboardState(NULL);
					if(keys[SDL_SCANCODE_LCTRL] && !keys[SDL_SCANCODE_LSHIFT])
					{
						// moves right one char
						if(row != this->text[col].size())
						{
							row++;
						}
					}
				}
				break;
				case SDLK_RIGHT:
				{
					// moves right one char
					if(row != this->text[col].size())
					{
						row++;
					}
				}
				break;
				case SDLK_h:
				{
					const Uint8* keys = SDL_GetKeyboardState(NULL);
					if(keys[SDL_SCANCODE_LCTRL] && !keys[SDL_SCANCODE_LSHIFT])
					{
						// moves left one char
						if(row != 0)
						{
							row--;
						}
					}
				}
				break;
				case SDLK_LEFT:
				{
					// moves left one char
					if(row != 0)
					{
						row--;
					}
				}
				break;
				case SDLK_b:
				{
					const Uint8* keys = SDL_GetKeyboardState(NULL);
					if(keys[SDL_SCANCODE_LCTRL])
					{
						if(row != 0)
						{
							for(char* c = &text[col][row]; *c != ' ';
								c = &text[col][row + 1])
							{
								if(row == 0)
								{
									if(col > 0)
									{
										col--;
										row = text[col].size();
									}
									break;
								}
								else
								{
									row--;
								}
							}
						}
						else
						{
							if(col > 0)
							{
								col--;
								row = text[col].size();
							}
						}
					}
				}
				break;
				case SDLK_n:
				{
					const Uint8* keys = SDL_GetKeyboardState(NULL);
					if(keys[SDL_SCANCODE_LCTRL])
					{
						for(char* c = &text[col][row]; *c != ' ';
							c = &text[col][row - 1])
						{
							if(row == text[col].size())
							{
								if(col + 1 < text.size())
								{
									col++;
									row = 0;
								}
								break;
							}
							else
							{
								row++;
							}
						}
					}
				}
				break;
				case SDLK_TAB:
				{
					this->text[col].insert(row, "    ");
					row += 4;
					changed = true;
				}
				break;
				case SDLK_d:
				{
					const uint8_t* keys = SDL_GetKeyboardState(NULL);
					if(keys[SDL_SCANCODE_LCTRL])
					{
						for(; text[col][row] != ' ' && row < text[col].size();)
						{
							text[col].erase(text[col].begin() + row);
						}
						if(text[col][row] == ' ')
						{
							text[col].erase(text[col].begin() + row);
						}
						changed = true;
					}
				}
				break;
				case SDLK_r:
				{
					// reload
					const uint8_t* keys = SDL_GetKeyboardState(NULL);
					if(keys[SDL_SCANCODE_LCTRL])
					{
						read(std::string("res/test/test.txt"));
						changed = true;
					}
				}
				break;
				case SDLK_F4:
				{
					save(std::string("res/test/test.txt"));
				}
				break;
			}
		}
		break;
	}
}

void Editor::read(std::string path)
{
	this->text.clear();
	std::ifstream ifstream;
	ifstream.open(path);
	if(ifstream.is_open())
	{
		for(std::string line; std::getline(ifstream, line);)
		{
			this->text.push_back(line);
		}
		ifstream.close();
	}
}

void Editor::save(std::string path)
{
	std::ofstream ofstream;
	ofstream.open(path);
	if(ofstream.is_open())
	{
		for(std::string line : this->text)
		{
			ofstream << line << '\n';
		}
		ofstream.close();
	}
}
