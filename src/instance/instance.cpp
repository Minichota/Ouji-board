#include <SDL2/SDL.h>

#include "instance.hpp"
#include "system.hpp"

Instance::Instance(Ivec pos, Ivec size, short border_size) :
pos(pos), size(size)
{
	this->thread = std::thread(&Instance::thread_update, this);
	this->border_size = border_size;
	this->render_size = { size.x - 2 * border_size, size.y - 2 * border_size };
}

Instance::~Instance()
{
	thread.join();
}

void Instance::thread_update()
{
	while(true)
	{
		update();
		SDL_Delay(1000.0f / 144.0f);
	}
}

void Instance::update()
{
}

void Instance::render()
{
	SDL_Rect rect = { pos.x + border_size, pos.y + border_size, render_size.x,
					  render_size.y };
	SDL_Point p = SDL_Point{ mouse_pos.x, mouse_pos.y };
	if(SDL_PointInRect(&p, &rect))
	{

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	}
	SDL_RenderFillRect(renderer, &rect);
}
