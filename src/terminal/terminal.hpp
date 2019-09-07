#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include "renderable.hpp"
#include "sizeable.hpp"
#include "text_renderer.hpp"
#include "input_handling.hpp"
#include "event_handler.hpp"
#include "SDL_FontCache.h"

class Terminal : public Window {
	public:
	Terminal(SDL_Renderer* const renderer, int x, int y, unsigned short w, unsigned short h, const char* font_path);
	~Terminal();

	void render();
	void update();

	void handle_event(const SDL_Event e);

	private:
	static constexpr SDL_Color cursor_color {255, 255, 255, 255};
	SDL_Rect cursor_pos;

	Text_Renderer text_renderer;
	Input_Handler input_handler;
};
#endif
