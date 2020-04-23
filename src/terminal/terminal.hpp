#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <SDL2/SDL_ttf.h>
#include <vector>

#include "instance.hpp"
#include "tty.hpp"

class Terminal : public Instance
{
	public:
	Terminal(Ivec pos, Ivec size, short border_size, SDL_Color border_color,
			 SDL_Color font_color);
	~Terminal();

	void update() override;
	void render() override;

	void process_event(const SDL_Event& event) override;

	void handle_resize();

	private:
	std::vector<std::string> text;
	Ivec cursor_pos;
	Ivec glyph_size;

	SDL_Color font_color;
	bool changed;

	tty_instance* tty;
};

#endif
