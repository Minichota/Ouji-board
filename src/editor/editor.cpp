#include <fstream>

#include "editor.hpp"
#include "system.hpp"

Editor::Editor(Ivec pos, Ivec size, short border_size, std::string file,
			   SDL_Color border_color, SDL_Color font_color) :
Instance(pos, size, border_size, border_color),
animation(Ivec(pos.x + border_size, pos.y + border_size), 1000,
		  Resources::create_text("SAVED", Resources::MONO,
								 SDL_Color{ 255, 255, 255, 255 }),
		  SINGULAR)
{
	this->font = Resources::get_font(Resources::MONO);
	this->font_color = font_color;
	this->changed = true;
	this->row = 0;
	this->col = 0;
	SDL_StartTextInput();
	this->scroll_chars = { 0, 0 };
	this->curr_file = file;

	// TODO change file location
	read(curr_file);
}

void Editor::update()
{
	while((int)row - scroll_chars.x >= num_cells.x - 1)
	{
		this->scroll_chars.x += 2;
		this->changed = true;
	}
	while((int)row - scroll_chars.x < 0)
	{
		this->scroll_chars.x -= 10;
		if(scroll_chars.x < 0)
		{
			this->scroll_chars.x = 0;
			break;
		}
		this->changed = true;
	}
	while((int)col - scroll_chars.y >= num_cells.y - 1)
	{
		this->scroll_chars.y += 2;
		this->changed = true;
	}
	while((int)col - scroll_chars.y < 0)
	{
		this->scroll_chars.y -= 10;
		if(scroll_chars.y < 0)
		{
			this->scroll_chars.y = 0;
			break;
		}
		this->changed = true;
	}
}

void Editor::render()
{
	// rendering outline
	Instance::render();

	handle_resize();
	// rendering cursor

	TTF_SizeText(font, " ", &glyph_size.x, &glyph_size.y);
	if(changed && text.size() > 0)
	{
		this->num_cells = Ivec((size.x - border_size * 2) / glyph_size.x,
							   (size.y - border_size * 2) / glyph_size.y);
		if(render_texture != nullptr)
		{
			SDL_DestroyTexture(render_texture);
		}
		// a new blank texture
		SDL_Texture* render_complete = SDL_CreateTexture(
			SDL::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			(size.x - border_size * 2), (size.y - border_size * 2));

		// setting to draw to blank texture
		SDL_SetRenderTarget(SDL::renderer, render_complete);

		// rendering each layer to blank texture
		for(size_t i = 0; i < text.size(); i++)
		{
			SDL_Texture* texture_part =
				Resources::create_text(text[i], Resources::MONO, font_color);
			SDL_Rect line_pos;
			SDL_QueryTexture(texture_part, nullptr, nullptr, &line_pos.w,
							 &line_pos.h);
			line_pos.x = -scroll_chars.x * glyph_size.x;
			line_pos.y = i * line_pos.h - scroll_chars.y * glyph_size.y;
			SDL_RenderCopy(SDL::renderer, texture_part, NULL, &line_pos);
			SDL_DestroyTexture(texture_part);
		}
		// assigning blank texture to classes texture and resetting renderer
		this->render_texture = render_complete;
		SDL_SetRenderTarget(SDL::renderer, NULL);
		this->changed = false;
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
	animation.render();
}

void Editor::process_event(const SDL_Event& event)
{
	switch(Instance::state)
	{
		case NORMAL:
		{
			const Uint8* keys = SDL_GetKeyboardState(NULL);
			switch(event.type)
			{
				case SDL_TEXTINPUT:
				{
					// normal text
					for(char* c = (char*)event.text.text; *c != '\0'; c++)
					{
						this->text[col].insert(text[col].begin() + row, *c);
						this->row++;
					}
					this->changed = true;
				}
				break;
				case SDL_KEYDOWN:
				{
					SDL_StartTextInput();
					switch(event.key.keysym.sym)
					{
						case SDLK_BACKSPACE:
						{
							if(text[col].size() > 0 && row != 0)
							{
								// removal of single char
								this->row--;
								this->text[col].erase(text[col].begin() + row);
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
									this->row = text[col].size() - copy.size();
								}
								else
								{
									text.erase(text.begin() + col + 1);
								}
							}
							this->changed = true;
						}
						break;
						case SDLK_RETURN:
						{
							if(keys[SDL_SCANCODE_LSHIFT])
							{
								this->text.emplace(text.begin() + col);
								this->changed = true;
								this->row = 0;
							}
							else
							{
								// inserts new line
								std::string copy_to_end = text[col].substr(row);
								this->text[col].erase(text[col].begin() + row,
													  text[col].end());
								this->col++;
								this->row = 0;
								this->changed = true;
								this->text.emplace(text.begin() + col);
								this->text[col].append(copy_to_end);
							}
						}
						break;
						case SDLK_k:
						{
							if(keys[SDL_SCANCODE_LCTRL] &&
							   !keys[SDL_SCANCODE_LSHIFT])
							{
								// moves up one char
								if(col != 0)
								{
									this->col--;
									if(row > text[col].size())
									{
										this->row = text[col].size();
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
								this->col--;
								if(row > text[col].size())
								{
									this->row = text[col].size();
								}
							}
						}
						break;
						case SDLK_j:
						{
							if(keys[SDL_SCANCODE_LCTRL] &&
							   !keys[SDL_SCANCODE_LSHIFT])
							{
								// moves down one char
								this->col++;
								if(col > text.size() - 1)
								{
									this->col = text.size() - 1;
								}
								if(row > text[col].size())
								{
									this->row = text[col].size();
								}
							}
						}
						break;
						case SDLK_DOWN:
						{
							// moves down one char
							this->col++;
							if(col > text.size() - 1)
							{
								this->col = text.size() - 1;
							}
							if(row > text[col].size())
							{
								this->row = text[col].size();
							}
						}
						break;
						case SDLK_l:
						{
							if(keys[SDL_SCANCODE_LCTRL] &&
							   !keys[SDL_SCANCODE_LSHIFT])
							{
								// moves right one char
								if(row != text[col].size())
								{
									this->row++;
								}
							}
						}
						break;
						case SDLK_RIGHT:
						{
							// moves right one char
							if(row != text[col].size())
							{
								this->row++;
							}
						}
						break;
						case SDLK_h:
						{
							if(keys[SDL_SCANCODE_LCTRL] &&
							   !keys[SDL_SCANCODE_LSHIFT])
							{
								// moves left one char
								if(row != 0)
								{
									this->row--;
								}
							}
						}
						break;
						case SDLK_LEFT:
						{
							// moves left one char
							if(row != 0)
							{
								this->row--;
							}
						}
						break;
						case SDLK_b:
						{
							bool next_word_flag = false;
							if(keys[SDL_SCANCODE_LCTRL])
							{
								for(char* c = &text[col][row];
									*c == ' ' || !next_word_flag;
									c = &text[col][row])
								{
									if(row == 0)
									{
										if(col > 0)
										{
											this->col--;
											this->row = text[col].size();
										}
										break;
									}
									else
									{
										if(*c == ' ')
										{
											next_word_flag = true;
										}
										this->row--;
									}
								}
							}
						}
						break;
						case SDLK_n:
						{
							if(keys[SDL_SCANCODE_LCTRL])
							{
								bool next_word_flag = false;
								for(char* c = &text[col][row];
									*c == ' ' || !next_word_flag;
									c = &text[col][row])
								{
									if(row == text[col].size())
									{
										if(col + 1 < text.size())
										{
											this->col++;
											this->row = 0;
										}
										break;
									}
									else
									{
										if(*c == ' ')
										{
											next_word_flag = true;
										}
										row++;
									}
								}
							}
						}
						break;
						case SDLK_TAB:
						{
							this->text[col].insert(row, "    ");
							this->row += 4;
							this->changed = true;
						}
						break;
						case SDLK_d:
						{
							if(keys[SDL_SCANCODE_LCTRL])
							{
								for(; text[col][row] != ' ' &&
									  row < text[col].size();)
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
					}
				}
				break;
			}
			break;
		}
		break;
		case COMMAND:
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_r:
				{
					// reload
					read(std::string(curr_file));
					this->changed = true;
					Instance::state = NORMAL;
				}
				break;
				case SDLK_s:
				{
					// save file
					save(curr_file);
					Instance::state = NORMAL;
					animation.activate();
				}
				break;
			}
			SDL_StopTextInput();
		}
		break;
	}
}

void Editor::handle_resize()
{
	if(render_texture != nullptr)
	{
		Ivec texture_size;
		SDL_QueryTexture(render_texture, nullptr, nullptr, &texture_size.x,
						 &texture_size.y);
		if(Ivec(size.x - border_size * 2, size.y - border_size * 2) !=
		   texture_size)
		{
			this->changed = true;
		}
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
		for(std::string line : text)
		{
			ofstream << line << '\n';
		}
		ofstream.close();
	}
}
