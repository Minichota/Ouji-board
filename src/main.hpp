#ifndef MAIN_HPP
#define MAIN_HPP
#include <vector>
#include "window.hpp"
int main();
void handle_event(const SDL_Event e);
extern bool should_close;
static std::vector<Window> windows;
#endif
