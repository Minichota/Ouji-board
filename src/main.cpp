#include <SDL2/SDL.h>

#include "vectors.hpp"

/*
   global variables
*/

SDL_Window* window;
SDL_Renderer* renderer;
const Ivec window_size = { 1920, 1080 };

static void load_sdl()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Video init failed: %s\n", SDL_GetError());
		exit(-1);
	}
	window = SDL_CreateWindow("Ouji board", SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED, window_size.x,
							  window_size.y, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, 0);
}

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
	}
}

int main()
{
	load_sdl();
	while(true)
	{
		process_events();

		/* render */
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	return 0;
}
