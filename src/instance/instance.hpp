#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <thread>

#include "vectors.hpp"

class Instance
{
	public:
	Instance(Ivec pos, Ivec size, short border_size);
	~Instance();
	void thread_update();
	virtual void update();
	virtual void render();

	virtual void process_event(const SDL_Event& event);

	bool collides(Ivec m_pos);

	bool active;

	protected:
	Ivec pos;
	Ivec size;
	short border_size;
	Ivec render_size;

	private:
	std::thread thread;
};

#endif
