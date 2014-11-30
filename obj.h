#ifndef _OBJ_H_
#define _OBJ_H_

#include "stdafx.h"
#include <vector>
#include <string>
#include <map>

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
	struct mtl {
		bool KaSet;
		GLfloat Ka[4];
		GLfloat Kd[4];
		GLfloat Ks[4];
		int illum;
		GLfloat Tr, Ns;
		mtl() {
			KaSet = 0;
			Ka[0] = Ka[1] = Ka[2] = 0.2;
			Kd[0] = Kd[1] = Kd[2] = 0.8;
			Ks[0] = Ks[1] = Ks[2] = 1.0;
			Ka[3] = Kd[3] = Ks[3] = 1.0;
			illum = 1;
			Tr = 1.0; Ns = 0.0;
		}
	};
	std::map <std::string, mtl> mtlTable;
	void loadMTL(std::string);
public:
    void read(std::string);
    void draw();
	void clear();
};

#endif
