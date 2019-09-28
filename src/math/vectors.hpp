#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <stdlib.h>
#include <ostream>

template <class T>
class Vector
{
	public:
	Vector(T x, T y):
	x(x),
	y(y)
	{
	}
	Vector():
	x(0),
	y(0)
	{
	}
	~Vector()
	{
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector& x)
	{
		os << " x: " << x.x << " y: " << x.y << " ";
		return os;
	}

	T x;
	T y;
};
typedef Vector<float>  Fvec;
typedef Vector<short>  Svec;
typedef Vector<int>    Ivec;
typedef Vector<double> Dvec;
typedef Vector<long>   Lvec;
typedef Vector<size_t> STvec;
typedef Vector<unsigned short> USvec;
#endif
