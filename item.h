#ifndef _ITEM_
#define _ITEM_

#include "obj.h"
#include "vec.h"

class Item
{
public:
	Vec3i loc;
	objModel obj;
	int type;
	bool draw;
	flt scale;
	Vec3f p;
	Item(Vec3i a, const char *filename, int t, flt scale, Vec3f p) :scale(scale), p(p){
		loc = a;
		obj.read(filename);
		type = t;
		draw = true;
	}
	void set_false(){
		draw = false;
	}
};

#endif