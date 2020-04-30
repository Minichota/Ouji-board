#include "text-buffer.hpp"
#include "system.hpp"

TextBuffer::TextBuffer(Ivec pos, Ivec size, short border_size, char* data_read,
					   SDL_Color border_color, SDL_Color font_color) :
Instance(pos, size, border_size, border_color)
{
	this->font_color = font_color;
	this->text = data_read;
	this->data_read = data_read;
	set_text(data_read);
}

TextBuffer::~TextBuffer()
{
	// clear data_read on close
	*data_read = '\0';
}

void TextBuffer::update()
{
}

void TextBuffer::render()
{
	Instance::render();
	if(strcmp(text.c_str(), data_read) != 0)
	{
		set_text(std::string(data_read));
	}
	handle_resize();
	SDL_Rect dest = { pos.x + border_size, pos.y + border_size,
					  (size.x - border_size * 2), (size.y - border_size * 2) };
	SDL_RenderCopy(SDL::renderer, render_texture, NULL, &dest);
}

void TextBuffer::process_event(const SDL_Event& event)
{
	switch(Instance::state)
	{
		case NORMAL:
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_c:
						{
							*data_read = '\0';
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
		}
		break;
		case DEBUG:
		{
		}
		break;
	}
}

void TextBuffer::set_text(std::string data)
{
	std::vector<std::string> text = Util::split_string(data, '\n');
	if(render_texture != nullptr)
	{
		SDL_DestroyTexture(render_texture);
		// set to nullptr as this function is called on resize
		// and can be a problem due to multi-threading.
		render_texture = nullptr;
	}
	SDL_Texture* render_complete = SDL_CreateTexture(
		SDL::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		(size.x - border_size * 2), (size.y - border_size * 2));
	SDL_SetRenderTarget(SDL::renderer, render_complete);

	Ivec glyph_size;
	TTF_SizeText(Resources::get_font(Resources::MONO), " ", &glyph_size.x,
				 &glyph_size.y);
	for(size_t i = 0; i < text.size(); i++)
	{
		SDL_Texture* texture =
			Resources::create_text(text[i], Resources::MONO, font_color);

		SDL_Rect dest_rect = { 0, (int)i * glyph_size.y };
		SDL_QueryTexture(texture, nullptr, nullptr, &dest_rect.w, &dest_rect.h);

		SDL_RenderCopy(SDL::renderer, texture, nullptr, &dest_rect);

		SDL_DestroyTexture(texture);
	}

	SDL_SetRenderTarget(SDL::renderer, NULL);
	this->render_texture = render_complete;
	this->text = data;
}

void TextBuffer::clear()
{
	*data_read = '\0';
}

void TextBuffer::handle_resize()
{
	if(render_texture != nullptr)
	{
		Ivec texture_size;
		SDL_QueryTexture(render_texture, nullptr, nullptr, &texture_size.x,
						 &texture_size.y);
		if(Ivec(size.x - border_size * 2, size.y - border_size * 2) !=
		   texture_size)
		{
			// reset text
			set_text(text);
		}
	}
}
