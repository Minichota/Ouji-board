#include <iostream>
// for initializing and sutdown functions 
#include <SDL2/SDL.h> 
// for rendering images and graphics on screen 
#include <SDL2/SDL_image.h> 
// for using SDL_Delay() functions 
#include <SDL2/SDL_timer.h> 

#include "main.h"
int main()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
		printf("error initializing SDL: %s\n", SDL_GetError()); 
	} 
	SDL_Window* win = SDL_CreateWindow("Ouji Board", 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			1000, 1000, SDL_WINDOW_RESIZABLE); 
	SDL_Renderer* rend;

	rend = SDL_CreateRenderer(win, -1, 0);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	bool should_close = false;
	while(!should_close)
	{
		SDL_Event e;
		while(SDL_PollEvent(&e) > 0)
		{
			handle_event(e, should_close);
			SDL_UpdateWindowSurface(win);			
		}
		// clear the window
		SDL_RenderClear(rend);
		// draw the window
		SDL_RenderPresent(rend);

		SDL_Delay(1000.0f/60.0f);
	}
	return 0;
}

void handle_event(SDL_Event e, bool& should_close)
{
	switch(e.type)
	{
		case SDL_QUIT:
		{
			should_close = true;
			break;
		}
		case SDL_KEYDOWN:
		{
			std::cout << "pressed key " << SDL_GetKeyName(e.key.keysym.sym) << std::endl;
			switch(e.key.keysym.sym)
			{
				case SDLK_ESCAPE:
				{	
					should_close = true;	
				} break;
			}
		}
	}
}
