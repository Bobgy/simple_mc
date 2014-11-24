#ifndef _ENTITY_H
#define _ENTITY_H

#include <cstring>
#include <cmath>
#include <algorithm>
#include <cassert>
#include "vec.h"

const flt CLOCK_T = 1.0 / 30;
const flt RESISTANCE = 0.92;
const flt GRAVITY = 1.2;
const flt EPS_COLLIDE = 1e-2;

inline bool in_range(flt x, flt low, flt high, bool lowEq = 0, bool highEq = 0){
	if (lowEq && zero(x - low)) return 1;
	if (highEq && zero(x - high)) return 1;
	if (low > high) std::swap(low, high);
	return x >= low + EPS_COLLIDE && x <= high - EPS_COLLIDE;
}
inline void get_quadrant(flt x, flt y, int &dir, int &dt){ //find the face facing (x, y)
	if (fabs(x) > fabs(y)){
		dir = 0;
		dt = x > 0;
	}
	else {
		dir = 2;
		dt = y > 0;
	}
}

//test if (x, y) is in circle O(0,0) with radius r
inline bool test_point_in_circle(flt x, flt y, flt r){
	return x*x + y*y < r*r - EPS_COLLIDE;
}

//test if circle O(cx,cy) with radius r intersects with rectangle [0, lx] * [0, ly]
inline bool test_circle_rectangle_intersect(flt cx, flt cy, flt r, flt lx, flt ly){
	static bool u, v;
	u = in_range(cx, 0, lx), v = in_range(cy, 0, ly);
	if (u && v) return 1;
	for (int i = 0; i <= 1; ++i)
		for (int j = 0; j <= 1; ++j)
			if (test_point_in_circle(i*lx - cx, j*ly - cy, r)) return 1;
	for (int i = -1; i <= 1; i += 2){
		if (in_range(i*r + cx, 0, lx) && in_range(cy, 0, ly)) return 1;
		if (in_range(cx, 0, lx) && in_range(i*r + cy, 0, ly))return 1;
	}
	return 0;
}

//return the intersection length of [a1,a2] and [b1,b2]
inline flt seg_intersect(flt a1, flt a2, flt b1, flt b2){
	flt a = std::max(a1, b1), b = std::min(a2, b2);
	if (a < b) return b - a;
	return 0;
}

class Entity{

private:
	inline bool zero(flt x){ return fabs(x) < EPS_COLLIDE; }
	inline int sgn(flt x){ return fabs(x) < EPS_COLLIDE ? 0 : (x > 0 ? 1 : -1); }
	inline bool in_block(flt x, int u){ return x >= u && x <= u + 1; }

	Vec3f p;     //Three dimensional coordinates (x, y, z)
	Vec3f v;     //The speed vector (vx, vy, vz)
	flt mass_inv; //The inverse of mass: mass_inv = 1 / mass
	bool G;       //Is this entity afftected by gravity?
	bool M;       //Can this entity move?
	flt r, h;     //Collision shape as a cylinder with radius r and height h.

public:

	bool on_ground;  //Is this entity on the ground?

	operator Vec3f() const { return p; }

	Entity(Vec3f p, Vec3f v, flt r, flt h, flt mass_inv, bool G = 1, bool M = 1) :
		p(p), v(v), mass_inv(mass_inv), G(G), M(M), r(r), h(h){}

	flt operator[](size_t id) const {
		return p[id];
	}

	//move to (p[0],p[1],p[2]) directly
	void move(Vec3f _p){
		if (M) p = _p;
	}

	void give_velocity(flt _p[3], flt len){
		if (M) {
			for (int i = 0; i < 3; ++i) if (!zero(_p[i]))
				v[i] = (v[i] + _p[i] * len * 3) / 4;
		}
	}

	void be_slowed(flt resistance){
		if (M) v = v * resistance;
	}

	//be given an force of _F
	void force(Vec3f _F){
		if (M) v = v + _F * (mass_inv * CLOCK_T);
	}

	//fall because of gravity
	void fall(){
		if (M && G) v[1] -= GRAVITY * CLOCK_T;
	}

	//update to simulate the movement in time seconds
	void update(){
		if (M) {
			p = p + v;
			v = v * RESISTANCE;
		}
	}

	//calculate the collision with a cube with its lower coordinates at x
	//returns whether x is exactly on the block
	bool collide_cube_vertically(const Vec3i x);

	void collide_cube_horizontally(const Vec3i x);
};

#endif