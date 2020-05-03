#include <SDL2/SDL.h>

#include "instance.hpp"
#include "system.hpp"
#include "traces.hpp"

State Instance::state = NORMAL;
Instance::Instance(Ivec pos, Ivec size, short border_size,
				   SDL_Color border_color)
{
	this->pos = pos;
	this->size = size;
	this->continue_thread = true;
	this->border_size = border_size;
	this->border_color = border_color;
	this->active = false;
	this->changed = true;
	this->render_texture = nullptr;
	this->thread = std::thread(&Instance::thread_update, this);
	push_trace(true, "/instances/positions/pos_x", &this->pos.x);
	push_trace(true, "/instances/positions/pos_y", &this->pos.y);
	push_trace(true, "/instances/sizes/size_x", &this->size.x);
	push_trace(true, "/instances/sizes/size_y", &this->size.y);
}

Instance::~Instance()
{
	this->continue_thread = false;
	this->thread.join();
	SDL_DestroyTexture(render_texture);
}

void Instance::thread_update()
{
	while(continue_thread)
	{
		if(active)
		{
			update();
		}
		SDL_Delay(1000.0f / 144.0f);
	}
}

void Instance::update()
{
}

void Instance::render()
{
	if(active)
	{
		SDL_SetRenderDrawColor(SDL::renderer, border_color.r, border_color.g,
							   border_color.b, border_color.a);
	}
	else
	{
		SDL_SetRenderDrawColor(SDL::renderer, 0, 0, 0, 255);
	}
	SDL_Rect rect = { pos.x, pos.y, size.x, size.y };
	SDL_RenderFillRect(SDL::renderer, &rect);
}

void Instance::process_event(const SDL_Event& event)
{
}

bool Instance::collides(Ivec m_pos)
{
	return m_pos.x > pos.x && m_pos.x < pos.x + size.x && m_pos.y > pos.y &&
		   m_pos.y < pos.y + size.y;
}
