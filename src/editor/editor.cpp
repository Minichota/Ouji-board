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
}

void Editor::render()
{
	if(changed && text.size() > 0)
	{
		if(render_texture != nullptr)
		{
			SDL_DestroyTexture(render_texture);
		}
		const char* surface_text = text[0].c_str();
		SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(
			font, surface_text, SDL_Color{ 255, 0, 0, 255 }, size.x);
		render_texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		changed = false;
	}
	if(render_texture != nullptr)
	{
		SDL_Rect rect = { pos.x, pos.y };
		SDL_QueryTexture(render_texture, nullptr, nullptr, &rect.w, &rect.h);
		SDL_RenderCopy(renderer, render_texture, nullptr, &rect);

		SDL_Rect cursor;
		TTF_SizeText(font, " ", &cursor.w, &cursor.h);
		int char_count = size.x / cursor.w;
		int wrap_count = rect.h / cursor.h - 1;
		std::cout << char_count << " " << wrap_count << " " << row << std::endl;
		cursor.x = pos.x + cursor.w * (row - char_count * wrap_count);
		cursor.y = pos.y + cursor.h * wrap_count;

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &cursor);
	}
}

void Editor::process_event(const SDL_Event& event)
{
	switch(event.type)
	{
		case SDL_TEXTINPUT:
		{
			this->text[0].append(event.text.text);
			changed = true;
			row++;
		}
		break;
		case SDL_KEYDOWN:
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_BACKSPACE:
				{
					if(this->text[0].size() > 0)
					{
						this->text[0].pop_back();
						changed = true;
						row--;
					}
					else
					{
						// index--;
						// changed = true;
					}
				}
				break;
				case SDLK_RETURN:
				{
					// index++;
					// changed = true;
				}
				break;
			}
		}
		break;
	}
}
