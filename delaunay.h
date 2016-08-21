#ifndef DELAUNAY_H_INCLUDED
#define DELAUNAY_H_INCLUDED

/// Acknowledgement:
/// The files "delaunay.h" and "delaunay.cpp" include a minimally modified version of
/// Bl4ckb0ne's delaunay triangulation code. The original can be found at
/// https://github.com/Bl4ckb0ne/delaunay-triangulation.
/// Included below is the license under which the original was published.

/// If you are the owner the original code and fault my usage of it, please contact
/// me by any means.

/*
Copyright (c) 2015 Simon Zeni (simonzeni@gmail.com)


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:


The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

template <typename T>
class Vector2
{
	public:
		//
		// Constructors
		//

		Vector2()
		{
			x = 0;
			y = 0;
		}

		Vector2(T _x, T _y)
		{
			x = _x;
			y = _y;
		}

		Vector2(const Vector2 &v)
		{
			x = v.x;
			y = v.y;
		}

		void set(const Vector2 &v)
		{
			x = v.x;
			y = v.y;
		}

		//
		// Operations
		//
		T dist2(const Vector2 &v)
		{
			T dx = x - v.x;
			T dy = y - v.y;
			return dx * dx + dy * dy;
		}

		float dist(const Vector2 &v)
		{
			return sqrtf(dist2(v));
		}

		T x;
		T y;

};

template<typename T>
std::ostream &operator << (std::ostream &str, Vector2<T> const &point)
{
	return str << "Point x: " << point.x << " y: " << point.y;
}

template<typename T>
bool operator == (Vector2<T> v1, Vector2<T> v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y);
}

typedef Vector2<float> Vec2f;

class TriEdge
{
	public:
		TriEdge(const Vec2f &p1, const Vec2f &p2) : p1(p1), p2(p2) {};
		TriEdge(const TriEdge &e) : p1(e.p1), p2(e.p2) {};

		Vec2f p1;
		Vec2f p2;
};

inline std::ostream &operator << (std::ostream &str, TriEdge const &e)
{
	return str << "TriEdge " << e.p1 << ", " << e.p2;
}

inline bool operator == (const TriEdge & e1, const TriEdge & e2)
{
	return 	(e1.p1 == e2.p1 && e1.p2 == e2.p2) ||
			(e1.p1 == e2.p2 && e1.p2 == e2.p1);
}


class Triangle
{
	public:
		Triangle(const Vec2f &_p1, const Vec2f &_p2, const Vec2f &_p3);

		bool containsVertex(const Vec2f &v);
		bool circumCircleContains(const Vec2f &v);

		Vec2f p1;
		Vec2f p2;
		Vec2f p3;
		TriEdge e1;
		TriEdge e2;
		TriEdge e3;
};

inline std::ostream &operator << (std::ostream &str, const Triangle & t)
{
	return str << "Triangle:" << std::endl << "\t" << t.p1 << std::endl << "\t" << t.p2 << std::endl << "\t" << t.p3 << std::endl << "\t" << t.e1 << std::endl << "\t" << t.e2 << std::endl << "\t" << t.e3 << std::endl;

}

inline bool operator == (const Triangle &t1, const Triangle &t2)
{
	return	(t1.p1 == t2.p1 || t1.p1 == t2.p2 || t1.p1 == t2.p3) &&
			(t1.p2 == t2.p1 || t1.p2 == t2.p2 || t1.p2 == t2.p3) &&
			(t1.p3 == t2.p1 || t1.p3 == t2.p2 || t1.p3 == t2.p3);
}


typedef Vector2<float> Vec2f;

class Delaunay
{
	public:
		const std::vector<Triangle>& triangulate(std::vector<Vec2f> &vertices);
		const std::vector<Triangle>& getTriangles() const { return _triangles; };
		const std::vector<TriEdge>& getTriEdges() const { return _triedges; };

	private:
		std::vector<Triangle> _triangles;
		std::vector<TriEdge> _triedges;
};




#endif // DELAUNAY_H_INCLUDED
