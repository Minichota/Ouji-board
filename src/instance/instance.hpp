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
	void update();
	void render();

	private:
	std::thread thread;
	Ivec pos;
	Ivec size;
	short border_size;
	Ivec render_size;
};

#endif
