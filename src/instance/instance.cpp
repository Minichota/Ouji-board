#include <SDL2/SDL.h>

#include "instance.hpp"
#include "system.hpp"

Instance::Instance(Ivec pos, Ivec size, short border_size)
{
	this->pos = pos;
	this->size = size;
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

	SDL_Rect rect = { pos.x, pos.y, render_size.x, render_size.y };
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	SDL_RenderFillRect(renderer, &rect);
}

void Instance::process_event(const SDL_Event& event)
{
}
