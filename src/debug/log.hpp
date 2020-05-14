#ifndef LOG_HPP
#define LOG_HPP

#include <SDL2/SDL.h>
#include <string>

#include "system.hpp"

struct Log
{
	SDL_Texture* log;
	long long spawn_time;
};

SDL_Rect render_logs(Ivec window_pos);

void push_log(std::string log);

#endif
