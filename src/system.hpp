#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "vectors.hpp"

extern SDL_Renderer* renderer;
extern SDL_Window* window;
extern Ivec mouse_pos;
const static Ivec window_size = { 1920, 1080 };
void load_sdl();
void clear_sdl();

#endif
