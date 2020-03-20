#include "terminal.hpp"
#include "system.hpp"
#include "tty.hpp"

#include <unistd.h>

Terminal::Terminal(Ivec pos, Ivec size, short border_size,
				   SDL_Color border_color, SDL_Color font_color) :
Instance(pos, size, border_size, border_color)
{
	this->font = Resources::get_font(Resources::MONO);
	this->font_color = font_color;
	this->render_texture = nullptr;
	this->changed = true;
	SDL_StartTextInput();
	this->text.emplace_back();
}

Terminal::~Terminal()
{
	SDL_DestroyTexture(render_texture);
}

void Terminal::update()
{
	// loads text with new information from tty
	std::string tty_out = read_command();
	if(!tty_out.empty())
	{
		std::vector<std::string> output = Util::split_string(tty_out, '\n');
		if(output.size() > 1)
		{
			output.pop_back();
		}
		this->text.push_back(text[0]);
		for(std::string s : output)
		{
			this->text.push_back(s);

			constexpr char ESC = (char)27;
			std::string clear_seq = std::string() + ESC + "[H" + ESC + "[2J";

			if(s == clear_seq)
			{
				// flush terminal
				this->text.clear();
				this->text.emplace_back();
			}
		}
		this->text[0].clear();
		this->changed = true;
	}
}

void Terminal::render()
{
	Instance::render();
	// updates text
	if(changed)
	{
		// creates a large texture from all the lines of text
		if(this->render_texture != nullptr)
		{
			SDL_DestroyTexture(render_texture);
		}
		TTF_SizeText(font, " ", &glyph_size.x, &glyph_size.y);
		SDL_Texture* render_complete = SDL_CreateTexture(
			SDL::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			(size.x - border_size), (size.y - border_size));
		SDL_SetRenderTarget(SDL::renderer, render_complete);
		for(size_t i = 0; i < text.size(); i++)
		{
			const char* render_text = text[i].c_str();
			SDL_Surface* surface =
				TTF_RenderText_Blended(font, render_text, font_color);
			SDL_Texture* texture =
				SDL_CreateTextureFromSurface(SDL::renderer, surface);

			SDL_Rect dest_rect = { 0, (int)i * glyph_size.y };
			SDL_QueryTexture(texture, nullptr, nullptr, &dest_rect.w,
							 &dest_rect.h);

			SDL_RenderCopy(SDL::renderer, texture, nullptr, &dest_rect);

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
		this->render_texture = render_complete;
		SDL_SetRenderTarget(SDL::renderer, nullptr);
		this->changed = false;
	}
	// actual rendering of text
	if(render_texture != nullptr)
	{
		SDL_Rect dest_rect = { pos.x + border_size, pos.y + border_size,
							   (size.x - border_size), (size.y - border_size) };
		SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &dest_rect);
	}
	// rendering cursor
	SDL_Rect cursor{ pos.x + border_size + glyph_size.x * (int)text[0].size(),
					 pos.y + border_size, glyph_size.x, glyph_size.y };
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

void Terminal::process_event(const SDL_Event& event)
{
	switch(Instance::state)
	{
		case NORMAL:
		{
			switch(event.type)
			{
				case SDL_TEXTINPUT:
				{
					this->text[0].append(event.text.text);
					this->changed = true;
				}
				break;
				case SDL_KEYDOWN:
				{
					SDL_StartTextInput();
					switch(event.key.keysym.sym)
					{
						case SDLK_RETURN:
						{
							set_command(text[0]);
						}
						break;
						case SDLK_BACKSPACE:
						{
							if(text[0].size() > 0)
							{
								this->changed = true;
								this->text[0].pop_back();
							}
						}
						break;
						case SDLK_c:
						{
							const uint8_t* keys = SDL_GetKeyboardState(NULL);
							if(keys[SDL_SCANCODE_LCTRL])
							{
								this->text[0].clear();
								this->changed = true;
							}
						}
						break;
					}
				}
				break;
			}
		}
		break;
		case COMMAND:
		{
			SDL_StopTextInput();
		}
		break;
	}
}
