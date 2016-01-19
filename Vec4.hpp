#pragma once

#ifndef VEC4_HPP
#define VEC4_HPP


template<class T>
class Vec4
{
public:
	Vec4(void){ r = 1.f; g = 1.f; b = 1.f; a = 1.f; };
	Vec4(T ix,T iy,T iz,T ia): r(ix), g(iy), b(iz), a(ia){};
	~Vec4(void){};

	T r;
	T g;
	T b;
	T a;
};

typedef Vec4<float> Vec4f;
typedef Vec4<int> Vec4i;
typedef Vec4<double> Vec4d;

#endif