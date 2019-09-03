#include "terminal.hpp"
Terminal::Terminal(SDL_Renderer* const renderer, int x, int y, unsigned short w, unsigned short h, const char* font_path):
Window(renderer, x, y, w, h),
text_renderer(renderer, x, y, w, h, font_path)
{
}
Terminal::~Terminal()
{
}

void Terminal::update()
{
	text_renderer.set_text(input_handler.get_buffer().c_str());
	text_renderer.update();
}

void Terminal::render()
{
	text_renderer.render();
}

void Terminal::handle_event(const SDL_Event e)
{
	input_handler.handle_event(e);
}
