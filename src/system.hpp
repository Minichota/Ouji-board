#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <SDL2/SDL.h>
#include <vector>

#include "instance.hpp"
#include "vectors.hpp"

extern std::vector<Instance*> instances;
extern size_t current_instance;

extern SDL_Renderer* renderer;
extern SDL_Window* window;
extern Ivec mouse_pos;
const static Ivec window_size = { 1920, 1080 };
void load_sdl();
void clear_sdl();

#endif
