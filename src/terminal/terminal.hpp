#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <SDL2/SDL_ttf.h>
#include <vector>

#include "instance.hpp"

class Terminal : public Instance
{
	public:
	Terminal(Ivec pos, Ivec size, short border_size, SDL_Color border_color,
			 SDL_Color font_color);
	~Terminal();

	void update() override;
	void render() override;

	void process_event(const SDL_Event& event) override;

	private:
	std::vector<std::string> text;
	Ivec cursor_pos;
	Ivec glyph_size;

	TTF_Font* font;
	SDL_Color font_color;
	SDL_Texture* render_texture;
	bool changed;
};

#endif
