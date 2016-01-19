#pragma once

#ifndef VEC3_HPP 
#define VEC3_HPP

template<class T>
class Vec3
{
public:
	Vec3(void){ x = 0; y = 0; z = 0;};
	Vec3(T ix,T iy,T iz): x(ix), y(iy), z(iz){};
	~Vec3(void){};
	
	template<typename S>
	Vec3<T> operator*(S rhs) { return Vec3<T>( x * rhs , y * rhs , z * rhs ); } ;
	Vec3<T> operator+(Vec3<T> rhs){ Vec3<T> lhs=*this; lhs.x+=rhs.x; lhs.y+=rhs.y; lhs.z+=rhs.z; return lhs;}
	Vec3<T> operator-(Vec3<T> rhs){ Vec3<T> lhs=*this; lhs.x-=rhs.x; lhs.y-=rhs.y; lhs.z-=rhs.z; return lhs;}

	bool operator!=(Vec3<T> rhs) { return (( x != rhs.x )||( y != rhs.y)||(z != rhs.z)); };
	bool operator==(Vec3<T> rhs) { return (( x == rhs.x )&&( y == rhs.y)&&(z == rhs.z)); };
	void operator+=(Vec3<T> rhs) { x += rhs.x; y += rhs.y; z += rhs.z; } ;
	void operator-=(Vec3<T> rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; } ;
	
	void normalize();
	Vec3<T> crossProductWith(Vec3<T> rhs);
	
	T x;
	T y;
	T z;
};

typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;


template <class T>
Vec3<T> Vec3<T>::crossProductWith(Vec3<T> rhs)
{
	Vec3<T> product;

	product.x = (y * rhs.z) - (z * rhs.y);
	product.y = (z * rhs.x) - (x * rhs.z);
	product.z = (x * rhs.y) - (y * rhs.x);

	return product;
}


template <class T>
void Vec3<T>::normalize()
{
	T length = sqrt( x*x + y*y + z*z);

	x = x / length;
	y = y / length;
	z = z / length;
}

#endif