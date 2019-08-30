#ifndef VECTOR_HPP
#define VECTOR_HPP
template <class T>
class Vector
{
	public:
	Vector(T x, T y):
	x(x),
	y(y)
	{
	}
	~Vector()
	{
	}

	T x;
	T y;
};
typedef Vector<short>  Svec;
typedef Vector<int>    Ivec;
typedef Vector<double> Dvec;
typedef Vector<long>   Lvec;
typedef Vector<unsigned short> USvec;
#endif
