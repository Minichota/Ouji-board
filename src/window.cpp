#include "window.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_image.h"
Window::Window(SDL_Renderer* const renderer, int x, int y, unsigned short w, unsigned short h) :
Sizeable(x, y, w, h),
Renderable(renderer),
border_color{255,0,0,255},
border_size(5)
{
}

Window::~Window()
{
}

void Window::update()
{
}

void Window::render()
{
	SDL_RenderSetScale(renderer, 1, 1);
	// do other shite
	SDL_RenderSetScale(renderer, 5, 5);
	SDL_RenderDrawRect(renderer, &border);
}

void Window::kill()
{
}

void Window::resize(short new_width, short new_height)
{
	this->size.x = new_width;
	this->size.y = new_height;
	this->border = {pos.x / border_size, pos.y / border_size, size.x / border_size, size.y / border_size};
}
