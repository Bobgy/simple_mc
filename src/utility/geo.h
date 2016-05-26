#pragma once

#include "stdafx.h"

const flt CLOCK_T = 1.0f / 60.0f;
const flt RESISTANCE = 0.92f;
const flt GRAVITY = 1.2f;
const flt EPS_COLLIDE = 1e-2f;

inline bool in_range(flt x, flt low, flt high, bool lowEq = 0, bool highEq = 0) {
	if (lowEq && zero(x - low)) return 1;
	if (highEq && zero(x - high)) return 1;
	if (low > high) std::swap(low, high);
	return x >= low + EPS_COLLIDE && x <= high - EPS_COLLIDE;
}

inline void get_quadrant(flt x, flt y, int &dir, int &dt) { //find the face facing (x, y)
	if (fabs(x) > fabs(y)) {
		dir = 0;
		dt = x > 0;
	} else {
		dir = 2;
		dt = y > 0;
	}
}

//test if (x, y) is in circle O(0,0) with radius r
inline bool test_point_in_circle(flt x, flt y, flt r) {
	return x*x + y*y < r*r - EPS_COLLIDE;
}

//test if circle O(cx,cy) with radius r intersects with rectangle [0, lx] * [0, ly]
inline bool test_circle_rectangle_intersect(flt cx, flt cy, flt r, flt lx, flt ly) {
	static bool u, v;
	u = in_range(cx, 0, lx), v = in_range(cy, 0, ly);
	if (u && v) return 1;
	for (int i = 0; i <= 1; ++i)
		for (int j = 0; j <= 1; ++j)
			if (test_point_in_circle(i*lx - cx, j*ly - cy, r)) return 1;
	for (int i = -1; i <= 1; i += 2) {
		if (in_range(i*r + cx, 0, lx) && in_range(cy, 0, ly)) return 1;
		if (in_range(cx, 0, lx) && in_range(i*r + cy, 0, ly)) return 1;
	}
	return 0;
}

//return the intersection length of [a1,a2] and [b1,b2]
inline flt seg_intersect(flt a1, flt a2, flt b1, flt b2) {
	flt a = std::max(a1, b1), b = std::min(a2, b2);
	if (a < b) return b - a;
	return 0;
}
