#ifndef _VEC_H
#define _VEC_H

#include <utility>
#include <iostream>
#include <cmath>
using namespace std;
template <class T> inline T sqr(T x){ return x*x; }

template < class T >
class Vec3{

private:

	T x[3];

public:

	Vec3(T v[3]){
		x[0] = v[0];
		x[1] = v[1];
		x[2] = v[2];
	}
	Vec3(T a, T b, T c){
		x[0] = a;
		x[1] = b;
		x[2] = c;
	}
	Vec3(){}
	T *operator&(){
		return x;
	}
	const T *operator&() const {
		return x;
	}
	T &operator[](size_t id){
		return x[id];
	}
	T operator[](size_t id) const {
		return x[id];
	}
	bool operator < (Vec3 r) const {
		if (x[0] != r[0]) return x[0] < r[0];
		if (x[1] != r[1]) return x[1] < r[1];
		return x[2] < r[2];
	}
	Vec3<T> operator + (Vec3 r) const {
		return Vec3(x[0] + r[0], x[1] + r[1], x[2] + r[2]);
	}
	Vec3<T> operator + (T r) const {
		return Vec3(x[0] + r, x[1] + r, x[2] + r);
	}
	Vec3<T> operator - (Vec3 r) const {
		return Vec3(x[0] - r[0], x[1] - r[1], x[2] - r[2]);
	}
	Vec3<T> operator - (T r) const {
		return Vec3(x[0] - r, x[1] - r, x[2] - r);
	}
	friend Vec3<T> operator * (Vec3<T> l, T k) {
		return Vec3(l[0] * k, l[1] * k, l[2] * k);
	}
	friend Vec3<T> operator * (T k, Vec3<T> r){
		return Vec3(r[0] * k, r[1] * k, r[2] * k);
	}
	T operator * (Vec3<T> r) const { //dot product
		return x[0] * r[0] + x[1] * r[1] + x[2] * r[2];
	}
	Vec3<T> operator ^ (Vec3<T> r) const { //cross product
		return Vec3<T>(x[1] * r[2] - x[2] * r[1], x[2] * r[0] - x[0] * r[2], x[0] * r[1] - x[1] * r[0]);
	}
	T operator !() const {
		return sqrt(sqr(x[0]) + sqr(x[1]) + sqr(x[2]));
	}
	friend T sqr(Vec3<T> x) {
		return x * x;
	}
	void print(){
		cout << x[0] << '\t' << x[1] << '\t' << x[2] << endl;
	}
	Vec3<T> normalize() const {
		T len = !*this;
		return *this * (1/len);
	}

	template < class TT >
	Vec3<T>(Vec3<TT> r){
		x[0] = (T)r[0];
		x[1] = (T)r[1];
		x[2] = (T)r[2];
	}
};

typedef float flt;
const flt PI = acos(-1.0f);
const flt HALF_PI = PI / 2.0f;
const flt rad2deg = 180.0f / PI;

typedef Vec3<int> Vec3i;
typedef Vec3<flt> Vec3f;
typedef Vec3<double> Vec3fd;

const Vec3f Vec3fZero(0.0f, 0.0f, 0.0f);
const flt EPS = 1e-6f;

//test if x is zero under eps
inline bool zero(flt x){
	return fabs(x) < EPS;
}
inline int sgn(flt x){ return fabs(x) < EPS ? 0 : (x > 0 ? 1 : -1); }

const Vec3i FACE[6] = {
		{ 0, 1, 0 },  //top
		{ 0,-1, 0 },  //bottom
		{ 1, 0, 0 },  //front
		{-1, 0, 0 },  //back
		{ 0, 0, 1 },  //right
		{ 0, 0,-1 }   //left
};
const int FACE_AXIS[3][2] = {
		{ 3, 2 },
		{ 1, 0 },
		{ 5, 4 }
};
typedef std::pair<Vec3i, int> BlockAndFace;

//find the next integer in direction dir (-1/1)
inline int next_int(double x, int dir){
	if (dir > 0) return (int)ceil(x + 1e-10);
	else return (int)floor(x - 1e-10);
}

inline Vec3i floor(Vec3f x){
	return Vec3i((int)floor(x[0]), (int)floor(x[1]), (int)floor(x[2]));
}

inline Vec3i floor(Vec3fd x){
	return Vec3i((int)floor(x[0]), (int)floor(x[1]), (int)floor(x[2]));
}

inline Vec3i round(Vec3f x){
	return Vec3i((int)round(x[0]), (int)round(x[1]), (int)round(x[2]));
}

class Rotation {
private:
	// horizontal and vertical angle
	flt h, v;
public:
	Rotation(flt h, flt v) : h(h), v(v) {}
	void setRotation(flt h, flt v) {
		this->h = h;
		this->v = v;
	}
	flt getH() const {
		return h;
	}
	flt getV() const {
		return v;
	}
	Vec3f getFacingVector() const {
		return Vec3f(cos(h) * cos(v), sin(v), sin(h) * cos(v));
	}
	Vec3f getHorizontalFacingVector() const {
		return Vec3f(cos(h), 0.0f, sin(h));
	}
};

#endif
