#include <cstring>
#include <cmath>
#include <algorithm>
#include <cassert>
#include "vec.h"
#include "entity.h"

using namespace std;

//calculate the collision with a cube with its lower coordinates at x
//returns whether x is exactly on the block
bool Entity::collide_cube_vertically(Vec3i x){
	if (!test_circle_rectangle_intersect(p[0]-x[0],p[2]-x[2],r,1,1)) return false;
	// inelastic collision for top and bottom face
	if (in_range(p[1], x[1]+0.5, x[1]+1, false, true)) { //collide with top face
		p[1] = x[1] + 1;
		if (v[1] <= 0) {
			v[1] = 0;
			return true;
		}
	} else if (in_range(p[1] + h, x[1], x[1]+0.5, true, false)){ //collide with bottom face
		p[1] = x[1] - h;
		if (v[1] > 0) v[1] = 0;
	}
	return false;
}

//calculate the collision with a cube with its sides
void Entity::collide_cube_horizontally(const Vec3i x){
	flt len = seg_intersect(x[1], x[1] + 1, p[1], p[1] + h);
	if (zero(len)) return; //not vertically intersecting
	Vec3f cx;
	cx = Vec3f(x) + 0.5;
	if (zero(p[0] - cx[0]) && zero(p[2] - cx[2])) return; //the center coincide, cannot collide
	int a, b, dt;
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
		Vec3f p_corner;
		p_corner = x;
		p_corner[a] += dt;
		p_corner[b] = p[b] < cx[b] ? x[b] : x[b] + 1;
		Vec3f rel = p_corner - p;
		// the corner
		if (!test_point_in_circle(rel[0], rel[2], r)) return;
		rel = rel.normalize();
		//force((len - r - 1.0)*rel.normalize()); //elastic
		p = p_corner - rel * r;
		if (v*rel > 0) v = v - (v*rel)* rel;
	}
}

//test intersection
bool Entity::intersect_cube(Vec3i x){
	if (!test_circle_rectangle_intersect(p[0] - x[0], p[2] - x[2], r, 1, 1)) return false;
	if (zero(seg_intersect(x[1], x[1] + 1, p[1], p[1] + h)))return false;
	return true;
}