#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <vector>
#include <SDL2/SDL_image.h>
#include "vectors.hpp"
#include "sizeable.hpp"
#include "renderable.hpp"
#include "event_handler.hpp"

class Window : public Sizeable, public Renderable, Event_Handler {
	public:
	Window(SDL_Renderer* const renderer, int x, int y, unsigned short w, unsigned short h);
	virtual ~Window();

	virtual void update();
	virtual void render();
	virtual void kill();

	virtual void resize(short w, short h);
	virtual void handle_event(const SDL_Event event) = 0;

	protected:
	SDL_Rect border;
	const SDL_Color border_color;
	unsigned short border_size;
};
#endif
