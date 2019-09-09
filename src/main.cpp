#include <iostream>
// for initializing and shutdown functions
#include <SDL2/SDL.h>
// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>
// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

#include "main.hpp"
#include "terminal.hpp"
#include "error.hpp"
#include "TTY.hpp"

bool should_close = false;

int main()
{
	// initialize SDL
	{
		Error e = Error();
		if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			e.push_reason("error initializing SDL: ", SDL_GetError());
			e.set_error_code(1);
		}
		if(TTF_Init() != 0)
		{
			e.push_reason("error initializing TTF: ", SDL_GetError());
			e.set_error_code(1);
		}
	}


	SDL_Window* win = SDL_CreateWindow("Ouji Board",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			1000, 1000, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* rend;

	rend = SDL_CreateRenderer(win, -1, 0);

	Terminal term = Terminal(rend, 0, 0, 1000, 1000, "data/fonts/test.ttf");
	size_t width = 1000/32, height = 1000/32;
	TTY tty = TTY(width, height);

	// create window
	while(!should_close)
	{
		// handle events
		SDL_Event e;
		while(SDL_PollEvent(&e) > 0)
		{
			term.handle_event(e);
			handle_event(e);
			SDL_UpdateWindowSurface(win);
		}
		term.update();
		// clear the window
		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
		SDL_RenderClear(rend);

		// draw the window
		term.render();
		SDL_RenderPresent(rend);

		SDL_Delay(1000.0f/60.0f);
	}
	SDL_DestroyWindow(win);
	SDL_DestroyRenderer(rend);
	SDL_Quit();
	return 0;
}

void handle_event(const SDL_Event e)
{
	switch(e.type)
	{
		case SDL_QUIT:
		{
			Error err = Error();
			err.push_reason(USER_CLOSED, "window termination");
		} break;
		case SDL_KEYDOWN:
		{
			if((e.key.keysym.mod & SDLK_LCTRL) && e.key.keysym.sym == SDLK_c)
			{
				std::cout << "hit control+c" << std::endl;
			}
		}
	}
}
