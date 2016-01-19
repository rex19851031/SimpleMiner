#pragma once

#ifndef VEC2_HPP
#define VEC2_HPP

template <class T>
class Vec2
{
public:
	Vec2(void) : x(0) , y(0) {}
	Vec2(const T x,const T y){this->x = x; this->y = y;}
	~Vec2(void){}
	Vec2<T> operator+(const Vec2<T>& rv){ Vec2<T> lv=*this; lv.x+=rv.x; lv.y+=rv.y; return lv;}
	Vec2<T> operator+(const T& rv){ Vec2<T> lv=*this; lv.x+=rv; lv.y+=rv; return lv;}
	Vec2<T> operator-(const Vec2<T>& rv){ Vec2<T> lv=*this; lv.x-=rv.x; lv.y-=rv.y; return lv;}
	Vec2<T> operator-(const T& rv){ Vec2<T> lv=*this; lv.x-=rv; lv.y-=rv; return lv;}
	Vec2<T> operator*(const Vec2<T>& rv){ Vec2<T> lv=*this; lv.x*=rv.x; lv.y*=rv.y; return lv;}
	Vec2<T> operator*(const T& rv){ Vec2<T> lv=*this; lv.x*=rv; lv.y*=rv; return lv;}
	Vec2<T> operator/(const Vec2<T>& rv){ Vec2<T> lv=*this; lv.x/=rv.x; lv.y/=rv.y; return lv;}
	Vec2<T> operator/(const T& rv){ Vec2<T> lv=*this; lv.x/=rv; lv.y/=rv; return lv;}
	bool operator==(const Vec2<T>& rv) { Vec2<T> lv=*this; return (rv.x==lv.x && rv.y == lv.y); }
	bool operator!=(const Vec2<T>& rv) { Vec2<T> lv=*this; return (rv.x!=lv.x || rv.y != lv.y); }
	void setAsUnitVectorOfDegree(T degrees){ x=cos(degree2radians(degrees)); y=sin(degree2radians(degrees)); }
	float degree2radians(float degree){ return degree*0.0174f;  /* PI / 180 */ }
	T x,y;
};

typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int> Vec2i;

#endif