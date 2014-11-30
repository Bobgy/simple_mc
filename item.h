#ifndef _ITEM_
#define _ITEM_

#include "obj.h"
#include "vec.h"

class OBJ_ITEM
{
public:
	Vec3i loc;
	objModel obj;
	OBJ_ITEM *next;
	int type;
	bool draw;
	OBJ_ITEM(Vec3i a, string filename,int t)
	{
		loc = a;
		obj.read(filename);
		type = t;
		draw = true;
		next = NULL;
	}
	void set_false()
	{
		draw = false;
	}
	void set_next(OBJ_ITEM a)
	{
		next = &a;
	}
};


#endif