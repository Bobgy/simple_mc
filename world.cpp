#ifndef WORLD_H
#define WORLD_H
#include <map>
#include "vec.h"
using namespace std;
namespace world{
	typedef vec::Vec3<int> Vec3i;
	class World{
		map<Vec3i, int> block;
	};
}
#endif