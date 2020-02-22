#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "system.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Ivec mouse_pos = Ivec(0, 0);

void load_sdl()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		fprintf(stderr, "Video init failed: %s\n", SDL_GetError());
		exit(-1);
	}
	if(TTF_Init() < 0)
	{
		fprintf(stderr, "Video init failed: %s\n", SDL_GetError());
		exit(-1);
	}
	window = SDL_CreateWindow("Ouji board", SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED, window_size.x,
							  window_size.y, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, 0);
}

void clear_sdl()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	TTF_Quit();
}
