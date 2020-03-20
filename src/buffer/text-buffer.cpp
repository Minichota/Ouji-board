#include "text-buffer.hpp"
#include "system.hpp"

TextBuffer::TextBuffer(Ivec pos, Ivec size, short border_size, std::string data,
					   SDL_Color border_color, SDL_Color font_color) :
Instance(pos, size, border_size, border_color)
{
	this->font_color = font_color;
	this->text = Util::split_string(data, '\n');
	this->render_texture = nullptr;
	set_text(data);
}

TextBuffer::~TextBuffer()
{
}

void TextBuffer::update()
{
}

void TextBuffer::render()
{
	Instance::render();
	SDL_Rect dest = { pos.x + border_size, pos.y + border_size,
					  (size.x - border_size * 2), (size.y - border_size * 2) };
	SDL_RenderCopy(SDL::renderer, render_texture, NULL, &dest);
}

void TextBuffer::process_event(const SDL_Event& event)
{
}

void TextBuffer::set_text(std::string data)
{
	this->text = Util::split_string(data, '\n');
	if(render_texture != nullptr)
	{
		SDL_DestroyTexture(render_texture);
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
		const char* render_text = text[i].c_str();
		SDL_Surface* surface = TTF_RenderText_Blended(
			Resources::get_font(Resources::MONO), render_text, font_color);
		SDL_Texture* texture =
			SDL_CreateTextureFromSurface(SDL::renderer, surface);

		SDL_Rect dest_rect = { 0, (int)i * glyph_size.y };
		SDL_QueryTexture(texture, nullptr, nullptr, &dest_rect.w, &dest_rect.h);

		SDL_RenderCopy(SDL::renderer, texture, nullptr, &dest_rect);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	SDL_SetRenderTarget(SDL::renderer, NULL);
	this->render_texture = render_complete;
}
