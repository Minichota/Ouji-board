#include <vector>

#include "instance.hpp"

extern std::vector<Instance*> views;

void push_view(Instance* view);
void pop_view(Instance* view);

void render_views();
void handle_view_event(const SDL_Event& event);
