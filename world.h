#ifndef WORLD_H
#define WORLD_H

#include <map>
#include "vec.h"
#include "block.h"

using namespace std;

class World{

private:

	map<Vec3i, Block*> blocks;

public:

	//get the block at (p[0],p[1],p[2]), NULL means AIR block
	Block* get_block(Vec3i p) const;

	//returns the first block within radius r that is seen by an eye
	//at p looking at the direction vector dir
	//returns -1 in face to indicate no block is found
	block_and_face look_at_block(Vec3f p, Vec3f dir, flt r) const;
};
#endif