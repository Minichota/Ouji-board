#include "terminal.hpp"
#include "system.hpp"
#include "tty.hpp"

#include <unistd.h>

Terminal::Terminal(Ivec pos, Ivec size, short border_size) :
Instance(pos, size, border_size)
{
	this->font = Resources::get_font(Resources::MONO);
	this->render_texture = nullptr;
	this->changed = true;
	SDL_StartTextInput();
	this->text.emplace_back();
}

Terminal::~Terminal()
{
	SDL_DestroyTexture(render_texture);
}

std::vector<std::string> split_string(const std::string& str,
									  const std::string& delimiter)
{
	// TODO fix hacky solution
	std::vector<std::string> strings;

	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while((pos = str.find(delimiter, prev)) != std::string::npos)
	{
		strings.push_back(str.substr(prev, pos - prev));
		prev = pos + 1;
	}

	// To get the last substring (or only, if delimiter is not found)
	strings.push_back(str.substr(prev));

	return strings;
}

void Terminal::update()
{
	std::string tty_out = read_command();
	if(!tty_out.empty())
	{
		std::vector<std::string> output = split_string(tty_out, "\n");
		for(std::string s : output)
		{
			this->text.push_back(s);
		}
		this->changed = true;
	}
}

void Terminal::render()
{
	Instance::render();
	if(changed)
	{
		if(this->render_texture != nullptr)
		{
			SDL_DestroyTexture(render_texture);
		}
		TTF_SizeText(font, " ", &glyph_size.x, &glyph_size.y);
		SDL_Texture* render_complete = SDL_CreateTexture(
			SDL::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			render_size.x, render_size.y);
		SDL_SetRenderTarget(SDL::renderer, render_complete);
		for(size_t i = 0; i < text.size(); i++)
		{
			const char* render_text = text[i].c_str();
			SDL_Surface* surface = TTF_RenderText_Blended(
				font, render_text, SDL_Color{ 255, 0, 0, 255 });
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
	if(render_texture != nullptr)
	{
		SDL_Rect dest_rect = { pos.x + border_size, pos.y + border_size,
							   render_size.x, render_size.y };
		SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &dest_rect);
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
