#ifndef WORLD_H
#define WORLD_H

#include <map>
#include <vector>
#include "vec.h"
#include "block.h"
#include "entity.h"

using namespace std;

class World{

private:

	map<Vec3i, Block*> blocks;
	vector<Block> block_list;

public:

	typedef map<Vec3i, Block*>::iterator MapBlockIterator;

	//get the block at (p[0],p[1],p[2]), NULL means AIR block
	Block* get_block(Vec3i p) const;

	//returns the first block within radius r that is seen by an eye
	//at p looking at the direction vector dir
	//returns -1 in face to indicate no block is found
	block_and_face look_at_block(Vec3f p, Vec3f dir, flt r) const;
	
	//generate a world by random seed: seed
	World(int seed);

	map<Vec3i, Block*>::const_iterator begin() const {
		return blocks.begin();
	}
	map<Vec3i, Block*>::const_iterator end() const {
		return blocks.end();
	}
	map<Vec3i, Block*>::const_iterator find(Vec3i p) const {
		return blocks.find(p);
	}
};
#endif