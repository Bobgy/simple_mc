#ifndef _OBJ_H_
#define _OBJ_H_

#include <vector>
#include <string>
#include "glut.h"

class objModel {
private:
	struct vertex3f {
		GLfloat x, y, z;
	};
	struct vertex2f {
		GLfloat x, y;
	};
    std::vector <vertex3f> V, VN;
	std::vector <vertex2f> VT;
	std::vector <GLuint> vList, vtList, vnList;
	typedef std::vector <std::string> face;
	std::vector <face> F;
	int faceVertexCnt;
	void parse();
public:
    void read(const char *filename);
    void draw();
	void clear();
};

#endif
