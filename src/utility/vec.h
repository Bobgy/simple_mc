#ifndef _VEC_H
#define _VEC_H

#include "stdafx.h"

#include <utility>
#include <iostream>
#include <cmath>

#include "utility/def.h"

using namespace std;
template <typename T> inline T sqr(T x){ return x*x; }

template < typename T >
struct Vec3{
	T x[3];

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
		return Vec3{x[0] + r[0], x[1] + r[1], x[2] + r[2]};
	}
	Vec3<T> operator + (T r) const {
		return Vec3{x[0] + r, x[1] + r, x[2] + r};
	}
	template <typename TT>
	Vec3<T> &operator += (TT r) {
		return *this = *this + r;
	}
	Vec3<T> operator - (Vec3 r) const {
		return Vec3{x[0] - r[0], x[1] - r[1], x[2] - r[2]};
	}
	Vec3<T> operator - (T r) const {
		return Vec3{x[0] - r, x[1] - r, x[2] - r};
	}
	template <typename TT>
	Vec3<T> &operator -= (TT r) {
		return *this = *this - r;
	}
	friend Vec3<T> operator * (Vec3<T> l, T k) {
		return Vec3{l[0] * k, l[1] * k, l[2] * k};
	}
	friend Vec3<T> operator * (T k, Vec3<T> r){
		return Vec3{r[0] * k, r[1] * k, r[2] * k};
	}
	template <typename TT>
	Vec3<T> &operator *= (TT r) {
		return *this = *this * r;
	}
	Vec3<T> operator | (Vec3 r) const { //product of each axis
		return Vec3{x[0] * r[0], x[1] * r[1], x[2] * r[2]};
	}
	template <typename TT>
	Vec3<T> &operator |= (TT r) {
		return *this = *this | r;
	}
	T operator * (Vec3<T> r) const { //dot product
		return x[0] * r[0] + x[1] * r[1] + x[2] * r[2];
	}
	Vec3<T> operator ^ (Vec3<T> r) const { //cross product
		return Vec3<T>{x[1] * r[2] - x[2] * r[1], x[2] * r[0] - x[0] * r[2], x[0] * r[1] - x[1] * r[0]};
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
		if (sgn(len)) return *this * (1/len);
		//LOG_WARNING(__FUNCTION__, "normalizing zero vector!\n");
		return ZERO();
	}

	template <typename TT>
	operator Vec3<TT>() const {
		return Vec3<TT>{(TT)x[0], (TT)x[1], (TT)x[2]};
	}

	operator bool() const {
		return x[0] || x[1] || x[2];
	}

// static constants
	static Vec3<T> ZERO() { return Vec3{(T)0, (T)0, (T)0}; }
	static Vec3<T> X_AXIS() { return Vec3{(T)1, (T)0, (T)0}; }
	static Vec3<T> Y_AXIS() { return Vec3{(T)0, (T)1, (T)0}; }
	static Vec3<T> Z_AXIS() { return Vec3{(T)0, (T)0, (T)1}; }
	static Vec3<T> ONES() { return Vec3{(T)1, (T)1, (T)1}; }
};

typedef float flt;
const flt PI = acos(-1.0f);
const flt HALF_PI = PI / 2.0f;
const flt rad2deg = 180.0f / PI;

typedef Vec3<int> Vec3i;
typedef Vec3<flt> Vec3f;
typedef Vec3<double> Vec3fd;

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
	return Vec3i{(int)floor(x[0]), (int)floor(x[1]), (int)floor(x[2])};
}

inline Vec3i floor(Vec3fd x){
	return Vec3i{(int)floor(x[0]), (int)floor(x[1]), (int)floor(x[2])};
}

inline Vec3i round(Vec3f x){
	return Vec3i{(int)round(x[0]), (int)round(x[1]), (int)round(x[2])};
}

template < typename T >
struct Vec2 {

	T x[2];

	T *operator&() {
		return x;
	}
	const T *operator&() const {
		return x;
	}
	T &operator[](size_t id) {
		return x[id];
	}
	T operator[](size_t id) const {
		return x[id];
	}
	bool operator < (Vec2 r) const {
		if (x[0] != r[0]) return x[0] < r[0];
		return x[1] < r[1];
	}
	Vec2<T> operator + (Vec2 r) const {
		return Vec2{x[0] + r[0], x[1] + r[1]};
	}
	Vec2<T> operator + (T r) const {
		return Vec2{x[0] + r, x[1] + r};
	}
	template <typename TT>
	Vec2<T> &operator += (TT r) {
		return *this = *this + r;
	}
	Vec2<T> operator - (Vec2 r) const {
		return Vec2{x[0] - r[0], x[1] - r[1]};
	}
	Vec2<T> operator - (T r) const {
		return Vec2{x[0] - r, x[1] - r};
	}
	template <typename TT>
	Vec2<T> &operator -= (TT r) {
		return *this = *this - r;
	}
	friend Vec2<T> operator * (Vec2<T> l, T k) {
		return Vec2{l[0] * k, l[1] * k};
	}
	friend Vec2<T> operator * (T k, Vec2<T> r) {
		return Vec2{r[0] * k, r[1] * k};
	}
	template <typename TT>
	Vec2<T> &operator *= (TT r) {
		return *this = *this * r;
	}
	Vec2<T> operator | (Vec2 r) const { //product of each axis
		return Vec2{x[0] * r[0], x[1] * r[1]};
	}
	template <typename TT>
	Vec2<T> &operator |= (TT r) {
		return *this = *this | r;
	}
	T operator * (Vec2<T> r) const { //dot product
		return x[0] * r[0] + x[1] * r[1];
	}
	T operator ^ (Vec2<T> r) const { //cross product
		return x[0] * r[1] - x[1] * r[0];
	}
	T operator !() const {
		return sqrt(sqr(x[0]) + sqr(x[1]));
	}
	friend T sqr(Vec2<T> x) {
		return x * x;
	}
	void print() {
		cout << x[0] << '\t' << x[1] << endl;
	}
	Vec2<T> normalize() const {
		T len = !*this;
		if (sgn(len)) return *this * (1 / len);
		//LOG_WARNING(__FUNCTION__, "normalizing zero vector!\n");
		return ZERO();
	}

	template <typename TT>
	operator Vec2<TT>() const {
		return Vec2<TT>((TT)x[0], (TT)x[1]);
	}

	operator bool() const {
		return x[0] || x[1];
	}

	static Vec2<T> ZERO() { return Vec2{(T)0, (T)0}; }
	static Vec2<T> X_AXIS() { return Vec2{(T)1, (T)0}; }
	static Vec2<T> Y_AXIS() { return Vec2{(T)0, (T)1}; }
	static Vec2<T> ONES() { return Vec2{(T)1, (T)1}; }
};

typedef Vec2<int> Vec2i;
typedef Vec2<flt> Vec2f;
typedef Vec2<double> Vec2fd;

template <typename T>
Vec2<T> horizontal_projection(Vec3<T> v) {
	return Vec2<T>(v[0], v[2]);
}

template <typename T>
Vec3<T> as_horizontal_projection(Vec2<T> v) {
	return Vec3<T>(v[0], (T)0, v[2]);
}

struct Rotation {
	// horizontal and vertical angle
	Vec2f ang = {0.0f, 0.0f};
	Rotation() = default;
	Rotation(flt h, flt v) : ang{h, v} {}
	void setRotation(Vec2f rotation) {
		ang = rotation;
	}
	void setRotation(flt h, flt v) {
		ang = {h, v};
	}
	flt getH() const {
		return ang[0];
	}
	flt getV() const {
		return ang[1];
	}
	Vec2f getVec2() const {
		return ang;
	}
	Vec3f getFacingVector() const {
		const flt h = ang[0], v = ang[1];
		return Vec3f{cos(h) * cos(v), sin(v), sin(h) * cos(v)};
	}
	Vec3f getHorizontalFacingVector() const {
		const flt h = ang[0], v = ang[1];
		return Vec3f{cos(h), 0.0f, sin(h)};
	}
};

inline flt clamp(flt x, flt low, flt high) {
	if (x > high) return high;
	if (x < low) return low;
	return x;
}

inline flt normalize_angle(flt x) {
	if (x > PI) {
		return x - ceil((x - PI) / (PI * 2.0f)) * (PI * 2.0f);
	} else if (x < -PI) {
		return x + ceil((-PI - x) / (PI * 2.0f)) * (PI * 2.0f);
	}
	return x;
}

#endif
