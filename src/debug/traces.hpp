#ifndef TRACES_HPP
#define TRACES_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <variant>

#include "vectors.hpp"

template <class T>
struct Trace
{
	T* value;
	T prev = -1;
	SDL_Texture* texture = nullptr;
};

extern std::vector<std::variant<Trace<float>*, Trace<int>*>> traces;

void render_traces();

template <class T>
void push_trace(T* trace)
{
	traces.push_back(new Trace<T>{trace});
}

void remove_trace(Trace<float>* trace);
void remove_trace(Trace<int>* trace);

#endif
