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
	std::string input_text;
	std::vector<std::string> text;
	Ivec cursor_pos;
	int scroll_pos;
	Ivec glyph_size;

	SDL_Color font_color;

	tty_instance* tty;
};

#endif
