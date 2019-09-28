#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include "renderable.hpp"
#include "sizeable.hpp"
#include "text_renderer.hpp"
#include "input_handling.hpp"
#include "event_handler.hpp"
#include "SDL_FontCache.h"
#include "tty.hpp"

class Terminal : public Window {
	public:
	Terminal(SDL_Renderer* const renderer, int x, int y, unsigned short w, unsigned short h, const char* font_path);
	~Terminal();

	void render();
	void update();

	void handle_event(const SDL_Event e);

	void exec_cmd(std::string cmd);

	private:
	static constexpr SDL_Color cursor_color {255, 255, 255, 255};
	SDL_Rect cursor_pos;

	Text_Renderer text_renderer;
	Input_Handler input_handler;
	TTY tty;
};
#endif
