#include <fstream>

#include "editor.hpp"
#include "system.hpp"
#include "file-selector.hpp"
#include "instance-control.hpp"

#define GROUPING_COUNT 4
const std::vector<std::string> Editor::groupings =
{
	"\"!@#$%^&*',;.",
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
	"(){}[]<>",
	" "
};

Editor::Editor(Ivec pos, Ivec size, short border_size, std::string file,
			   SDL_Color border_color, SDL_Color font_color) :
Instance(pos, size, border_size, border_color),
animation(Ivec(pos.x + border_size, pos.y + border_size), 1000,
		  Resources::create_text("SAVED", Resources::MONO,
								 SDL_Color{ 255, 255, 255, 255 }),
		  SINGULAR)
{
	this->font_color = font_color;
	this->row = 0;
	this->col = 0;
	this->prev_col = 0;
	SDL_StartTextInput();
	this->scroll_chars = { 0, 0 };
	this->curr_file = file;

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

	if((changed && text.size() > 0) ||
	   (col != prev_col && std::stoi(Settings::get_setting("highlight-line").value)))
	{
		TTF_SizeText(Resources::get_font(Resources::MONO), " ", &glyph_size.x, &glyph_size.y);
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
		for(size_t i = scroll_chars.y; i < text.size(); i++)
		{
			std::string render_text = text[i];
			if(std::stoi(Settings::get_setting("space-dots").value))
			{
				bool quote_flag = false;
				for(size_t j = 0; j < render_text.size(); j++)
				{
					if(render_text[j] == '"')
						quote_flag = !quote_flag;
					if(render_text[j] == ' ' && !quote_flag)
						render_text[j] = (char)'·';
				}
			}

			SDL_Texture* texture_part;
			if(i == col && std::stoi(Settings::get_setting("highlight-line").value))
			{
				render_text.push_back(' ');
				texture_part = Resources::create_shaded_text(
					render_text, Resources::MONO,
					{ (uint8_t)(255 - font_color.r),
					  (uint8_t)(255 - font_color.g),
					  (uint8_t)(255 - font_color.b) },
					font_color);
				text[i].pop_back();
			}
			else
			{
				texture_part = Resources::create_text(render_text,
													  Resources::MONO,
													  font_color);
			}
			SDL_Rect line_pos;
			SDL_QueryTexture(texture_part, nullptr, nullptr, &line_pos.w, &line_pos.h);
			line_pos.x = -scroll_chars.x * glyph_size.x;
			line_pos.y = i * line_pos.h - scroll_chars.y * glyph_size.y;
			// done rendering
			SDL_RenderCopy(SDL::renderer, texture_part, NULL, &line_pos);
			SDL_DestroyTexture(texture_part);

			// break if it's past viewable y
			if(line_pos.y > size.y - border_size * 2 && text[i].size() > 0)
				break;
		}
		// assigning blank texture to classes texture and resetting renderer
		SDL_SetRenderTarget(SDL::renderer, NULL);
		this->render_texture = render_complete;
		this->prev_col = col;
		this->changed = false;
	}
	if(render_texture != nullptr)
	{
		// rendering all text
		SDL_Rect rect = { pos.x + border_size, pos.y + border_size };
		SDL_QueryTexture(render_texture, nullptr, nullptr, &rect.w, &rect.h);
		SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &rect);
	}

	SDL_Rect cursor = {
		// raw pos                        scroll offset                   border offset
		pos.x + glyph_size.x * (int)row - scroll_chars.x * glyph_size.x + border_size,
		pos.y + glyph_size.y * (int)col - scroll_chars.y * glyph_size.y + border_size,
		glyph_size.x, glyph_size.y
	};

	SDL_SetRenderDrawColor(SDL::renderer,
						   (uint8_t)(255 - font_color.r * std::stoi(Settings::get_setting("highlight-line").value)),
						   (uint8_t)(255 - font_color.g * std::stoi(Settings::get_setting("highlight-line").value)),
						   (uint8_t)(255 - font_color.b * std::stoi(Settings::get_setting("highlight-line").value)),
						   100);
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
									this->row = text[col].size();
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

								// count the previous offset
								uint8_t indent_offset = 0;
								char* stepper = text[col].data();
								while(*stepper != '\0' && *stepper == ' ')
								{
									indent_offset++;
									stepper++;
								}
								this->col++;
								// apply
								this->text.emplace(text.begin() + col);
								for(int i = 0; i < indent_offset; i++)
								{
									this->text[col].push_back(' ');
								}
								this->row = indent_offset;
								this->changed = true;
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
							else if(keys[SDL_SCANCODE_LCTRL] &&
									keys[SDL_SCANCODE_LSHIFT])
							{
								// moves column up
								if(col != 0)
								{
									this->changed = true;
									std::swap(text[col], text[col - 1]);
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
							else if(keys[SDL_SCANCODE_LCTRL] &&
									keys[SDL_SCANCODE_LSHIFT])
							{
								// moves column up
								if(col != text.size() - 1)
								{
									this->changed = true;
									std::swap(text[col], text[col + 1]);
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
							if(keys[SDL_SCANCODE_LCTRL])
							{
								int char_group = -1;
								for(size_t i = 0; i < GROUPING_COUNT; i++)
								{
									if(std::find(groupings[i].begin(), groupings[i].end(),
												 text[col][row]) != groupings[i].end())
									{
										char_group = i;
									}
								}
								bool cont = true;
								while(cont)
								{
									if(row == 0)
									{
										if(col != 0)
											row = text[--col].size();
										break;
									}
									row--;
									for(size_t i = 0; i < GROUPING_COUNT; i++)
									{

										if(std::find(groupings[i].begin(), groupings[i].end(),
													 text[col][row]) != groupings[i].end())
										{
											if((int)i != char_group)
											{
												cont = false;
												break;
											}
										}
									}
									while(text[col][row] == ' ')
									{
										row--;
										cont = false;
										if(row <= 0)
										{
											row = 0;
											if(col != 0)
												row = text[--col].size();
											break;
										}
									}
								}
							}
						}
						break;
						case SDLK_n:
						{
							if(keys[SDL_SCANCODE_LCTRL])
							{
								int char_group = -1;
								for(size_t i = 0; i < GROUPING_COUNT; i++)
								{
									if(std::find(groupings[i].begin(), groupings[i].end(),
												 text[col][row]) != groupings[i].end())
									{
										char_group = i;
									}
								}
								bool cont = true;
								while(cont)
								{
									if(row >= text[col].size())
									{
										row = text[col].size();
										if(col != text.size() - 1)
										{
											row = 0;
											col++;
										}
										break;
									}
									row++;
									for(size_t i = 0; i < GROUPING_COUNT; i++)
									{
										if(std::find(groupings[i].begin(), groupings[i].end(),
													 text[col][row]) != groupings[i].end())
										{
											if((int)i != char_group)
											{
												cont = false;
												break;
											}
										}
									}
									while(text[col][row] == ' ')
									{
										row++;
										cont = false;
										if(row > text[col].size())
										{
											row = text[col].size();
											if(col != text.size() - 1)
											{
												row = 0;
												col++;
											}
											break;
										}
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
									this->text[col].erase(text[col].begin() + row);
								}
								if(text[col][row] == ' ')
								{
									this->text[col].erase(text[col].begin() + row);
								}
								this->changed = true;
							}
						}
						break;
						case SDLK_i:
						{
							if(keys[SDL_SCANCODE_LCTRL])
							{
								this->text.insert(text.begin() + ++col, "");
								this->row = 0;
								this->changed = true;
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
				case SDLK_o:
				{
					FileSelector* fs = new FileSelector(Ivec(0,0), Ivec(0,0), 5);
					switch_instance(this, fs);
					return;
				}
				break;
			}
			SDL_StopTextInput();
		}
		break;
		case DEBUG:
		{
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
	animation.set_pos(pos + border_size);
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

std::vector<std::string> Editor::serialize()
{
	return {
		std::to_string(pos.x) + " " + std::to_string(pos.y),
		std::to_string(size.x) + " " + std::to_string(size.y),
		curr_file,
		"lc: " + std::to_string(text.size())
	};
}
