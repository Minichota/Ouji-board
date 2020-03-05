#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include <iostream>
#include <ostream>
#include <stdlib.h>

template <class T>
class Vector
{
	public:
	Vector(T x, T y) : x(x), y(y)
	{
	}
	Vector() : x(0), y(0)
	{
	}
	template <class X>
	Vector(Vector<X>&& old) : x(old.x), y(old.y)
	{
	}
	template <class X>
	Vector(Vector<X>& old) : x(old.x), y(old.y)
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

	Vector<T> operator/(const int& other)
	{
		return Vector<T>(x / other, y / other);
	}

	template <class other_type>
	Vector<T> operator/(const Vector<other_type>& other)
	{
		return Vector<T>(x / other.x, y / other.y);
	}

	template <class other_type>
	Vector<T> operator*(const Vector<other_type>& other)
	{
		return Vector<T>(x * other.x, y * other.y);
	}

	Vector<T> operator*(const double other)
	{
		return Vector<T>(x * other, y * other);
	}

	Vector<T> operator*(const float other)
	{
		return Vector<T>(x * other, y * other);
	}

	template <class other_type>
	Vector<T>& operator+=(const Vector<other_type>& other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

	template <class other_type>
	Vector<T> operator+(const Vector<other_type>& other)
	{
		return Vector<T>(x + other.x, y + other.y);
	}

	Vector<T> operator+(const int other)
	{
		return Vector<T>(x + other, y + other);
	}

	template <class other_type>
	Vector<T>& operator-=(const Vector<other_type>& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	template <class other_type>
	Vector<T> operator-(const Vector<other_type>& rhs) const
	{
		Vector<T> ret;
		ret.x = x - rhs.x;
		ret.y = y - rhs.y;
		return ret;
	}

	template <class other_type>
	bool operator!=(const Vector<other_type>& other)
	{
		return x != other.x || y != other.y;
	}

	template <class other_type>
	bool operator==(const Vector<other_type>& other)
	{
		return x == other.x && y == other.y;
	}

	T x;
	T y;
};
typedef Vector<float> Fvec;
typedef Vector<short> Svec;
typedef Vector<int> Ivec;
typedef Vector<double> Dvec;
typedef Vector<long> Lvec;
typedef Vector<size_t> STvec;
typedef Vector<unsigned short> USvec;
#endif
