#include <SDL2/SDL.h>

#include "editor.hpp"
#include "instance-control.hpp"
#include "system.hpp"
#include "terminal.hpp"
#include "tty.hpp"
#include "vectors.hpp"

using namespace SDL;
using namespace Resources;

static bool process_events()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				return false;
			}
			break;
		}
		handle_events(event);

		instances[current_instance]->process_event(event);
	}
	return true;
}
int main()
{
	load_sdl();
	load_res();
	Ivec instance_size = { window_size.x / 2, window_size.y };
	Editor win =
		Editor(Ivec(0, 0), window_size, 5, SDL_Color{ 255, 255, 255, 255 },
			   SDL_Color{ 255, 255, 255, 255 });
	Terminal win2 = Terminal(Ivec(instance_size.x, 0), instance_size, 5,
							 SDL_Color{ 255, 255, 255, 255 },
							 SDL_Color{ 255, 255, 255, 255 });
	instances.push_back(&win);
	// instances.push_back(&win2);

	current_instance = 0;
	instances[current_instance]->active = true;

	size_t normal_cache =
		Resources::cache_text(Resources::create_text("NORMAL MODE", MONO));
	size_t command_cache =
		Resources::cache_text(Resources::create_text("COMMAND MODE", MONO));

	start_tty();

	while(true)
	{
		if(!process_events())
		{
			break;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		for(size_t i = 0; i < instances.size(); i++)
		{
			instances[i]->render();
		}

		/* rendering of current_state to corner of screen */
		SDL_Texture* texture = nullptr;
		switch(Instance::state)
		{
			case NORMAL:
			{
				texture = Resources::load_cache_text(normal_cache);
			}
			break;
			case COMMAND:
			{
				texture = Resources::load_cache_text(command_cache);
			}
			break;
		}
		SDL_Rect dest_rect;
		SDL_QueryTexture(texture, nullptr, nullptr, &dest_rect.w, &dest_rect.h);
		dest_rect.x = 0;
		dest_rect.y = window_size.y - dest_rect.h;
		SDL_RenderCopy(renderer, texture, nullptr, &dest_rect);

		SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
		/* render */
		SDL_RenderPresent(renderer);

		SDL_Delay(1000.0f / 144.0f);
	}

	clear_res();
	clear_sdl();
	stop_tty();
	return 0;
}
