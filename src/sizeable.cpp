#include "sizeable.hpp"

Sizeable::Sizeable(int x, int y, unsigned short w, unsigned short h):
size(w, h),
pos(x, y)
{
}

Sizeable::~Sizeable()
{
}

Ivec Sizeable::get_pos()
{
	return this->pos;
}

USvec Sizeable::get_size()
{
	return this->size;
}
