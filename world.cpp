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
	static int sign[3], i;
	for (i = 0; i < 3; ++i)
		sign[i] = sgn(dir[i]);
	assert(sign[0] || sign[1] || sign[2]);
	Vec3f now(p);
	Vec3i p_block;
	int MAX_COUNT = 200;
	while(MAX_COUNT--){
		int axis = -1;
		flt next_time = 1e100; //infinite
		//find the next intersection point with an interger face
		for (i = 0; i < 3; ++i){
			if (sign[i] == 0) continue;
			int ni = next_int(now[i], sign[i]);
			flt t = (ni - now[i]) / dir[i];
			assert(t >= 0);
			if (t < next_time){
				next_time = t;
				axis = i;
			}
		}
		assert(~axis);
		now = now + (next_time + 1e-6) * dir;
		if (sqr(now - p) > r*r) break;
		p_block = floor(now);
		auto it = blocks.find(p_block);
		if (it != blocks.end() && it->second->test_intersection(p, dir)){
			return make_pair(p_block, FACE_AXIS[axis][dir[axis] < 0]);
		}
	}
	assert(MAX_COUNT >= 0);
	return make_pair(p_block, -1); //-1 means not found
}

//generate a world by random seed: seed
World::World(int seed){
	srand(seed);
	block_list.push_back(Block(AIR));
	block_list.push_back(Block(DIRT));
	for (int i = -32; i <= 32; ++i)
		for (int j = -32; j <= 32; ++j)
			for (int k = 0; k <= 3; ++k){
				int t = (1 << (k + 2)) - 1;
				if (k == 0 || (rand()&t) == t) blocks[Vec3i(i, k, j)] = &block_list[DIRT];
				else break;
			}
}