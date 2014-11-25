#ifndef _VEC_H
#define _VEC_H

#include <utility>
#include <iostream>

typedef float flt;
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
};

typedef Vec3<int> Vec3i;
typedef Vec3<flt> Vec3f;

const flt EPS = 1e-5;

//test if x is zero under eps
inline bool zero(flt x){
	return fabs(x) < EPS;
}
inline int sgn(flt x){ return fabs(x) < EPS ? 0 : (x > 0 ? 1 : -1); }

const Vec3i FACE[6] = {
		{ 0, 1, 0 },  //top
		{ 0,-1, 0 },  //bottom
		{ 1, 0, 0 },  //right
		{-1, 0, 0 },  //left
		{ 0, 0, 1 },  //front
		{ 0, 0,-1 }   //back
};
const int FACE_AXIS[3][2] = {
		{ 3, 2 },
		{ 1, 0 },
		{ 5, 4 }
};
typedef std::pair<Vec3i, int> block_and_face;

//find the next integer in direction dir (-1/1)
inline int next_int(flt x, int dir){
	if (dir > 0) return ceil(x + EPS);
	else return floor(x - EPS);
}

inline Vec3i floor(Vec3f x){
	return Vec3i(floor(x[0]), floor(x[1]), floor(x[2]));
}

inline Vec3i round(Vec3f x){
	return Vec3i(round(x[0]), round(x[1]), round(x[2]));
}
#endif