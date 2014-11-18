#ifndef BLOCK_H
#define BLOCK_H
enum block_type{
	AIR,
	DIRT
};
class Block {
private:
	block_type type;
public:
	bool is_opaque(){
		return true;
	}
	block_type get_block_type() const {
		return type;
	}
	Block(block_type _t):type(_t){}

	//test if the ray starting from p with direction vector dir can intersect with this block
	bool test_intersection(Vec3f p, Vec3f dir) const {
		return true; //!!TODO: finish this function
	}
};
#endif