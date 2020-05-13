#ifndef TEMP_HPP
#define TEMP_HPP

#include <SDL2/SDL.h>
#include <string>
#include <vector>

#include "vectors.hpp"

struct Temp
{
	int id;
	std::string text;
	SDL_Texture* texture;
};

SDL_Rect render_temps(Ivec window_pos);

void push_temp(std::string temp);

void handle_temp_event(const SDL_Event& event);

#endif
