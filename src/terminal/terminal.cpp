#include <unistd.h>
#include <iostream>
#include <fstream>

#include "terminal.hpp"
#include "error.hpp"
Terminal::Terminal(SDL_Renderer* const renderer, int x, int y, unsigned short w, unsigned short h, const char* font_path):
Window(renderer, x, y, w, h),
cursor_pos{x + border_size, y + border_size, 2, 22},
text_renderer(renderer, x + border_size, y + border_size, w - border_size * 2, h - border_size * 2, font_path)
{
}

Terminal::~Terminal()
{
}

void Terminal::update()
{
	text_renderer.set_text(input_handler.get_text_buffer().c_str());
	text_renderer.update();

	// x offset calc
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
					exec_cmd(input_handler.get_cmd_buffer());
				}
			}
		}
	}
}

void Terminal::exec_cmd(std::string cmd)
{
	std::string data;
	FILE * stream;
	const int max_buffer = 512;
	char buffer[max_buffer];
	cmd.append(" 2>&1");
	stream = popen(cmd.c_str(), "r");
	if (stream) {
		while (!feof(stream))
			if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
		pclose(stream);
	}
	input_handler.get_text_buffer().append(data.c_str());
}
