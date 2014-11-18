#ifndef ENTITY_H

#define ENTITY_H

#include <cstring>
#include <cmath>
#include <algorithm>
#include <cassert>
namespace entity {

	using namespace std;

	typedef float flt;
	const flt CLOCK_T = 1.0 / 30;
	const flt RESISTANCE = 0.9;
	const flt GRAVITY = 1.0;
	const flt EPS_COLLIDE = 1e-2;

	inline bool zero(flt x){ return fabs(x) < EPS_COLLIDE; }
	inline int sgn(flt x){ return fabs(x) < EPS_COLLIDE ? 0 : (x > 0 ? 1 : -1); }
	inline bool in_block(flt x, int u){ return x >= u && x <= u + 1; }
	template <class T> inline T sqr(T x){ return x*x; }
	inline bool in_range(flt x, flt low, flt high, bool lowEq=0, bool highEq=0){
		if (lowEq && zero(x - low)) return 1;
		if (highEq && zero(x - high)) return 1;
		if (low > high) swap(low, high);
		return x >= low +EPS_COLLIDE && x <= high - EPS_COLLIDE;
	}
	inline void get_quadrant(flt x, flt y, int &dir, int &dt){ //find the face facing (x, y)
		if (fabs(x) > fabs(y)){
			dir = 0;
			dt = x > 0;
		} else {
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

		flt p[3];     //Three dimensional coordinates (x, y, z)
		flt v[3];     //The speed vector (vx, vy, vz)
		flt mass_inv; //The inverse of mass: mass_inv = 1 / mass
		bool G;       //Is this entity afftected by gravity?
		bool M;       //Can this entity move?
		flt r, h;     //Collision shape as a cylinder with radius r and height h.

	public:

		Entity(flt _p[3], flt _v[3], flt r, flt h, flt mass_inv, bool G = 1, bool M = 1) :
				mass_inv(mass_inv), G(G), M(M), r(r), h(h){
			memcpy(p, _p, sizeof(flt) * 3);
			memcpy(v, _v, sizeof(flt) * 3);
		}

		flt operator[](size_t id) const {
			return p[id];
		}

		//move to (p[0],p[1],p[2]) directly
		void move(flt _p[3]){
			if (M) memcpy(p, _p, sizeof(flt) * 3);
		}

		void give_velocity(flt _p[3], flt len){
			for (int i = 0; i < 3; ++i) if (!zero(_p[i])) {
				v[i] = (v[i] + _p[i] * len * 3) / 4;
			}
		}

		//be given an force of _F
		void force(flt _F[3]){
			v[0] += _F[0] * mass_inv * CLOCK_T;
			v[1] += _F[1] * mass_inv * CLOCK_T;
			v[2] += _F[2] * mass_inv * CLOCK_T;
		}

		//fall because of gravity
		void fall(){
			if (M && G) v[1] -= GRAVITY * CLOCK_T;
		}

		//update to simulate the movement in time seconds
		void update(){
			for (int i = 0; i < 3; ++i){
				p[i] += v[i];
				v[i] *= RESISTANCE; //simulate the resistance
			}
		}

		//calculate the collision with a cube with its lower coordinates at x[3]
		void collide_cube_vertically(const int x[3]){
			if (!test_circle_rectangle_intersect(p[0]-x[0],p[2]-x[2],r,1,1)) return;
			// inelastic collision for top and bottom face
			if (in_range(p[1], x[1]+0.5, x[1]+1, false, true)) { //collide with top face
				p[1] = x[1] + 1;
				if (v[1] < 0) v[1] = 0;
			} else if (in_range(p[1] + h, x[1], x[1]+0.5, true, false)){ //collide with bottom face
				p[1] = x[1] - h;
				if (v[1] > 0) v[1] = 0;
			}
		}

		void collide_cube_horizontally(const int x[3]){
			flt len = seg_intersect(x[1], x[1] + 1, p[1], p[1] + h);
			if (zero(len)) return; //not vertically intersecting
			static flt cx[3];
			cx[0] = x[0] + 0.5, cx[1] = x[1] + 0.5, cx[2] = x[2] + 0.5;
			if (zero(p[0] - cx[0]) && zero(p[2] - cx[2])) return; //the center coincide, cannot collide
			static int a, b, dt;
			get_quadrant(p[0] - cx[0], p[2] - cx[2], a, dt);
			b = a == 0 ? 2 : 0; //collide along "a" axis;
			assert(a != b && (a == 0 || a == 2) && (b == 0 || b == 2));
			int sig = dt ? 1 : -1;
			if (in_range(p[b], x[b], x[b] + 1, true, true)) {
				if (in_range(p[a] - sig*r, cx[a], x[a] + dt, false, true)){
					p[a] = x[a] + dt + sig * r;
					if ((v[a] > 0) != dt) v[a] = 0;
				}
			} else { //collide with the corner
				flt pos = min(fabs(p[b] - x[b] - 1), fabs(p[b] - x[b]));
				if (pos > r) return;
				pos = -sig * sqrt(r*r - pos*pos); //p[a]+pos is the coordinate at the corner
				if (in_range(pos, cx[a], x[a] + dt, false, true)) {
					p[a] += (x[a] + dt) - pos;
					if ((v[a] > 0) != dt) v[a] = 0;
				}
			}
		}

	};
}

#endif