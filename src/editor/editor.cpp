#include "editor.hpp"
#include "system.hpp"

Editor::Editor(Ivec pos, Ivec size, short border) : Instance(pos, size, border)
{
	font = TTF_OpenFont("res/font/RobotoMono-Regular.ttf", 32);
	if(!font)
	{
		fprintf(stderr, "failed to load font: %s", TTF_GetError());
		exit(-1);
	}
	this->render_texture = nullptr;
	this->changed = true;
	this->row = 0;
	this->col = 0;
	text.emplace_back();
	SDL_StartTextInput();
}

Editor::~Editor()
{
}

void Editor::update()
{
	if((int)row - scroll_chars.x >= num_cells.x)
	{
		scroll_chars.x++;
	}
	if((int)row - scroll_chars.x < 0)
	{
		scroll_chars.x -= 10;
		if(scroll_chars.x < 0)
		{
			scroll_chars.x = 0;
		}
	}
	if((int)col - scroll_chars.y >= num_cells.y)
	{
		scroll_chars.y++;
	}
	if((int)col - scroll_chars.y < 0)
	{
		scroll_chars.y -= 10;
		if(scroll_chars.y < 0)
		{
			scroll_chars.y = 0;
		}
	}
}

void Editor::render()
{
	if(changed && text.size() > 0)
	{
		TTF_SizeText(font, " ", &glyph_size.x, &glyph_size.y);
		this->num_cells = Ivec(size.x / glyph_size.x, size.y / glyph_size.y);
		if(render_texture != nullptr)
		{
			SDL_DestroyTexture(render_texture);
		}
		// a new blank texture
		SDL_Texture* render_complete =
			SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
							  SDL_TEXTUREACCESS_TARGET, size.x, size.y);

		// setting to draw to blank texture
		SDL_SetRenderTarget(renderer, render_complete);
		SDL_Surface* surface;
		SDL_Texture* texture_part;

		// rendering each layer to blank texture
		for(size_t i = 0; i < text.size(); i++)
		{
			const char* surface_text = text[i].c_str();
			surface = TTF_RenderText_Blended(font, surface_text,
											 SDL_Color{ 255, 0, 0, 255 });
			texture_part = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_Rect line_pos;
			SDL_QueryTexture(texture_part, nullptr, nullptr, &line_pos.w,
							 &line_pos.h);
			line_pos.x = pos.x - scroll_chars.x * glyph_size.x;
			line_pos.y = pos.y + i * line_pos.h - scroll_chars.y * glyph_size.y;
			SDL_RenderCopy(renderer, texture_part, NULL, &line_pos);
			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture_part);
		}
		// assigning blank texture to classes texture and resetting renderer
		render_texture = render_complete;
		SDL_SetRenderTarget(renderer, NULL);
		changed = false;
	}
	if(render_texture != nullptr)
	{
		SDL_Rect rect = { pos.x, pos.y };
		SDL_QueryTexture(render_texture, nullptr, nullptr, &rect.w, &rect.h);
		SDL_RenderCopy(renderer, render_texture, nullptr, &rect);
	}
	SDL_Rect cursor = {
		pos.x + glyph_size.x * (int)row - scroll_chars.x * glyph_size.x,
		pos.y + glyph_size.y * (int)col - scroll_chars.y * glyph_size.y,
		glyph_size.x, glyph_size.y
	};

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &cursor);
}

void Editor::process_event(const SDL_Event& event)
{
	switch(event.type)
	{
		case SDL_TEXTINPUT:
		{
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
						row--;
						this->text[col].erase(this->text[col].begin() + row);
					}
					else if(col != 0)
					{
						this->col--;
						if(text[col].size() != 0)
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
					std::string copy_to_end = this->text[col].substr(row);
					this->text[col].erase(this->text[col].begin() + row,
										  this->text[col].end());
					col++;
					row = 0;
					changed = true;
					this->text.emplace(this->text.begin() + col);
					this->text[col].append(copy_to_end);
				}
				break;
				case SDLK_UP:
				{
					if(col != 0)
					{
						col--;
					}
				}
				break;
				case SDLK_DOWN:
				{
					col++;
					if(col > this->text.size() - 1)
					{
						col = this->text.size() - 1;
					}
				}
				break;
				case SDLK_RIGHT:
				{
					if(row != this->text[col].size())
					{
						row++;
					}
				}
				break;
				case SDLK_LEFT:
				{
					if(row != 0)
					{
						row--;
					}
				}
				break;
			}
		}
		break;
	}
}
