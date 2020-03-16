#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <SDL2/SDL.h>
#include <thread>

#include "vectors.hpp"

enum State
{
	NORMAL,
	COMMAND,
};

class Instance
{
	public:
	Instance(Ivec pos, Ivec size, short border_size,
			 SDL_Color border_color = { 255, 255, 255, 255 });
	~Instance();
	void thread_update();
	virtual void update();
	virtual void render();

	virtual void process_event(const SDL_Event& event);

	bool collides(Ivec m_pos);

	bool active;

	static State state;

	protected:
	Ivec pos;
	Ivec size;
	short border_size;
	SDL_Color border_color;
	Ivec render_size;

	private:
	std::thread thread;
	bool continue_thread = true;
};

#endif
