#ifndef INSTANCE_CONTROL_HPP
#define INSTANCE_CONTROL_HPP

#include <SDL2/SDL.h>

#include "system.hpp"
#include "vectors.hpp"

void handle_events(const SDL_Event& event);

void push_instance(Instance* instance);
void remove_instance(size_t index);

#endif
