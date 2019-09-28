#include <unistd.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <pty.h>
#include <termios.h>
#include <csignal>
#include <poll.h>

#include "terminal.hpp"
#include "error.hpp"
termios saved_attributes;
termios tattr;

void reset_input(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_attributes);
}

Terminal::Terminal(SDL_Renderer* const renderer, int x, int y, unsigned short w, unsigned short h, const char* font_path):
Window(renderer, x, y, w, h),
cursor_pos{x + border_size, y + border_size, 2, 22},
text_renderer(renderer, x + border_size, y + border_size, w - border_size * 2, h - border_size * 2, font_path),
tty(w/32, h/32)
{

	tcgetattr(STDIN_FILENO, &saved_attributes);
	atexit(reset_input);

	tattr.c_lflag &= ~(ICANON | ECHO | ECHONL);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

Terminal::~Terminal()
{
	tty.destroy(SIGHUP);
	tty.clear();
}

void Terminal::update()
{
	struct pollfd p[2] = { { tty.get_fd(), POLLIN, 0 } };

	if(input_handler.is_out())
	{
		p[0].events |= POLLOUT;
	}

	int pollres = poll(p, 1, 30);
	Error e = Error();
	e.set_error_code(1);


	if(pollres < 0)
	{
		// end program
		e.push_reason("poll result failed", "");
		return;
	}

	if((p[0].revents & POLLIN) && !input_handler.is_out())
	{
		std::string tty_data = tty.get().first;
		if(!input_handler.handle_ansi_seq(tty_data))
		{
		}
		input_handler.get_text_buffer().append(tty_data);
	}

	if(p[0].revents & (POLLERR | POLLHUP))
	{
		// let main loop finish
		e.set_finish(true);
		e.push_reason("poll error", "");
	}

	text_renderer.set_text(input_handler.get_text_buffer());
	text_renderer.update();

	std::string text = text_renderer.get_text();
	FC_Font*    font = text_renderer.get_font();
	SDL_Rect offset = FC_GetCharacterOffset(font, text.size(), size.x, text.c_str());

	cursor_pos.x = pos.x + border_size + offset.x;
	cursor_pos.y = pos.y + border_size + offset.y;
}

void Terminal::render()
{
	Window::render();
	text_renderer.render();
	SDL_SetRenderDrawColor(renderer, cursor_color.r, cursor_color.g, cursor_color.b, cursor_color.a);
	SDL_RenderDrawRect(renderer, &cursor_pos);
}

void Terminal::handle_event(const SDL_Event e)
{
	input_handler.handle_event(e);
	switch(e.type)
	{
		case SDL_KEYDOWN:
		{
			switch(e.key.keysym.sym)
			{
				case SDLK_RETURN:
				{
					std::string out = input_handler.get_cmd_buffer();
					tty.send(out);
					input_handler.set_out(false);
				}
			}
		}
	}
}
