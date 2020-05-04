#ifndef TRACES_HPP
#define TRACES_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <variant>

#include "vectors.hpp"

template <class T>
struct Trace
{
	void* address;
	std::string path;
	T* value;
	SDL_Texture* texture = nullptr;
};

extern std::vector<std::variant<Trace<float>*, Trace<int>*>> traces;

void render_traces();

bool handle_trace_event(const SDL_Event& event);

template <class T>
void push_trace(void* address, std::string path, T* trace)
{
	traces.push_back(new Trace<T>{address, path, trace});
}

void remove_trace(void* address);

#endif
