#include <SDL2/SDL.h>

#include "editor.hpp"
#include "instance-control.hpp"
#include "system.hpp"
#include "vectors.hpp"

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
		handle_events(event);

		instances[current_instance]->process_event(event);
	}
}
int main()
{
	load_sdl();
	Ivec instance_size = { window_size.x / 2, window_size.y / 2 };
	Editor win = Editor(Ivec(0, 50), instance_size, 5);
	Editor win2 = Editor(Ivec(instance_size.x, 0), instance_size, 5);
	instances.push_back(&win);
	instances.push_back(&win2);

	current_instance = 0;
	instances[current_instance]->active = true;
	while(true)
	{
		process_events();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		for(size_t i = 0; i < instances.size(); i++)
		{
			instances[i]->render();
		}

		SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
		/* render */
		SDL_RenderPresent(renderer);

		SDL_Delay(1000.0f / 144.0f);
	}

	clear_sdl();
	return 0;
}
