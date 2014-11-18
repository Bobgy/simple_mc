#include <map>
#include <cassert>
#include "vec.h"
#include "block.h"
#include "world.h"

//get the block at (p[0],p[1],p[2]), NULL means AIR block
Block* World::get_block(Vec3i p) const {
	auto it = blocks.find(p);
	if (it != blocks.end()) return it->second;
	return NULL;
}
	
//returns the first block within radius r that is seen by an eye
//at p looking at the direction vector dir
//returns -1 in face to indicate no block is found
block_and_face World::look_at_block(Vec3f p, Vec3f dir, flt r) const {
	assert(!zero(dir[0]) || !zero(dir[1]) || !zero(dir[2]));
	static int sign[3], i;
	for (i = 0; i < 3; ++i)
		sign[i] = sgn(dir[i]);
	Vec3f now(p);
	Vec3i p_block;
	do {
		int axis = -1;
		flt next_time = 1e100; //infinite
		//find the next intersection point with an interger face
		for (i = 0; i < 3; ++i){
			if (sign[i] == 0) continue;
			int ni = next_int(p[i], sign[i]);
			flt t = (ni - p[i]) / dir[i];
			if (t < next_time){
				next_time = t;
				axis = i;
			}
		}
		assert(~axis);
		now = now + next_time * dir;
		now[axis] = next_int(p[axis], sign[axis]); //make sure no floating error occurs
		if (sqr(now - p) > r*r) break;
		p_block = floor(now);
		auto it = blocks.find(p_block);
		if (it != blocks.end() && it->second->test_intersection(p, dir)){
			return make_pair(p_block, FACE_AXIS[axis][dir[axis] < 0]);
		}
	} while (sqr(now - p) < r*r);
	return make_pair(p_block, -1); //-1 means not found
}