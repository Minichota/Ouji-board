#ifndef TRACES_HPP
#define TRACES_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <variant>

#include "vectors.hpp"

template <class T>
struct Trace
{
	void* owner;
	std::string path;
	T* value;
	SDL_Texture* texture = nullptr;
};

extern std::vector<std::variant<Trace<float>*, Trace<int>*>> traces;

SDL_Rect render_traces(Ivec window_pos);

void handle_trace_event(const SDL_Event& event);

template <class T>
void push_trace(void* owner, std::string path, T* trace)
{
	traces.push_back(new Trace<T>{owner, path, trace});
}

void remove_trace(void* owner);

#endif
