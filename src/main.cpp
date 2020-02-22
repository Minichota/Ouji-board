#include <SDL2/SDL.h>

#include "editor.hpp"
#include "instance.hpp"
#include "system.hpp"
#include "vectors.hpp"

std::vector<Instance*> instances;
size_t current_instance = 0;

static void process_events()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				exit(0);
			}
			break;
		}
		instances[current_instance]->process_event(event);
	}
}
int main()
{
	load_sdl();
	Ivec instance_size = { window_size.x / 2, window_size.y / 2 };
	Editor win = Editor(Ivec(0, 0), instance_size, 5);
	instances.push_back(&win);
	while(true)
	{
		process_events();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		win.render();

		SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
		/* render */
		SDL_RenderPresent(renderer);

		SDL_Delay(1000.0f / 144.0f);
	}

	clear_sdl();
	return 0;
}
