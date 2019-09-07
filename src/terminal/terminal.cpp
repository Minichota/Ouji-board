#include "terminal.hpp"
Terminal::Terminal(SDL_Renderer* const renderer, int x, int y, unsigned short w, unsigned short h, const char* font_path):
Window(renderer, x, y, w, h),
cursor_pos{x + border_size, y + border_size, 2, 35},
text_renderer(renderer, x + border_size, y + border_size, w - border_size * 2, h - border_size * 2, font_path)
{
}

Terminal::~Terminal()
{
}

void Terminal::update()
{
	text_renderer.set_text(input_handler.get_buffer().c_str());
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
}
