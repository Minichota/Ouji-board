#ifndef INSTANCE_VIEW_HPP
#define INSTANCE_VIEW_HPP

#include <vector>

#include "instance.hpp"

extern std::vector<Instance*> views;

void push_view(Instance* view);
void pop_view(Instance* view);

SDL_Rect render_views(Ivec window_pos);
void handle_view_event(const SDL_Event& event);

#endif
