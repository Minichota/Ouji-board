#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP
#include <SDL2/SDL.h>
class Event_Handler {
	public:
	virtual void handle_event(const SDL_Event event) = 0;
};
#endif
