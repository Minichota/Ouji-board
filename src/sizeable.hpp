#ifndef SIZEABLE_HPP
#define SIZEABLE_HPP
#include "math/vectors.hpp"
class Sizeable {
	public:
	Sizeable(int x, int y, unsigned short w, unsigned short h);
	virtual ~Sizeable();
	virtual USvec get_size();
	virtual Ivec get_pos();

	protected:
	USvec size;
	Ivec pos;
};
#endif
