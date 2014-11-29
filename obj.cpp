#include "obj.h"
#include "glut.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void objModel::parse()
{
	for (int i = 0; i < this->F.size(); i++) {
		face f = this->F[i];
		for (int j = 0; j < F[i].size(); j++) {
			string s = F[i][j];
			int slashCnt = 0;
			int sp1 = -1, sp2 = -1;
			for (int k = 0; k < s.size(); k++) {
				if (s[k] == '/') {
					slashCnt++;
					if (slashCnt == 1) sp1 = k;
					else sp2 = k;
				}
			}
			int indexV, indexVT, indexVN;
			if (slashCnt == 0) {
				sscanf_s(s.c_str(), "%d", &indexV);
				vList.push_back(indexV - 1);
			}
			else if (slashCnt == 1) {
				sscanf_s(s.c_str(), "%d/%d", &indexV, &indexVT);
				vList.push_back(indexV - 1);
				vtList.push_back(indexVT - 1);
			}
			else if (slashCnt == 2) {
				if (sp1 + 1 == sp2) { // no vt
					sscanf_s(s.c_str(), "%d//%d", &indexV, &indexVN);
					vList.push_back(indexV - 1);
					vnList.push_back(indexVN - 1);
				}
				else { // has vt
					sscanf_s(s.c_str(), "%d/%d/%d", &indexV, &indexVT, &indexVN);
					vList.push_back(indexV - 1);
					vtList.push_back(indexVT - 1);
					vnList.push_back(indexVN - 1);
				}
			}
			else {
				cerr << "Error while parsing slash of faces" << endl;
			}
		}
	}
}

void objModel::clear()
{
	V.clear(); VN.clear(); VT.clear(); vList.clear(); vtList.clear(); vnList.clear(); F.clear();
}

void objModel::read(const char *filename)
{

	std::string s;
	std::ifstream fin(filename);
	if (!fin) {
		cerr << "Error : No such OBJ file." << endl;
		return;
	}
	clear();
	int lineNum = 0;
	while (fin >> s) {
		lineNum++;
		if (s=="v") {
			vertex3f v;
			fin >> v.x >> v.y >> v.z;
			V.push_back(v);
		}	
		else if (s == "vt") {
			vertex2f v;
			getline(fin, s);
			istringstream tmp(s);
			tmp >> v.x >> v.y;
			VT.push_back(v);
		}
		else if (s == "vn") {
			vertex3f v;
			fin >> v.x >> v.y >> v.z;
			VN.push_back(v);
		}
		else if (s == "f") {
			face f;
			getline(fin, s);
			istringstream tmp(s);
			string t;
			while (tmp >> t) f.push_back(t);
			F.push_back(f);
		}
		else {
			getline(fin, s);
			//cerr << "Warning: Unrecognized grammar on line " << lineNum << endl;
		}
	}
	parse();
}

void objModel::draw()
{
	int index = 0; 
	bool hasVT = vtList.size();
	bool hasVN = vnList.size();
	for (int i = 0; i < F.size(); i++) {
		int faceVertexCnt = F[i].size();
		if (faceVertexCnt == 3) {
			glBegin(GL_TRIANGLES);
			for (int j = 0; j < faceVertexCnt; j++) {
				glVertex3f(V[vList[index]].x, V[vList[index]].y, V[vList[index]].z);
				if (hasVT) {
					glTexCoord2f(VT[vtList[index]].x, VT[vtList[index]].y);
				}
				if (hasVN) {
					glNormal3f(VN[vnList[index]].x, VN[vnList[index]].y, VN[vnList[index]].z);
				}
				index++;
			}
			glEnd();
		}
		else if (faceVertexCnt == 4) { // 4 vertexes
			glBegin(GL_QUADS);
			for (int j = 0; j < faceVertexCnt; j++) {
				glVertex3f(V[vList[index]].x, V[vList[index]].y, V[vList[index]].z);
				if (hasVT) {
					glTexCoord2f(VT[vtList[index]].x, VT[vtList[index]].y);
				}
				if (hasVN) {
					glNormal3f(VN[vnList[index]].x, VN[vnList[index]].y, VN[vnList[index]].z);
				}
				index++;
			}
			glEnd();
		}
		else { // more than 4 vertexes
			glBegin(GL_POLYGON);
			for (int j = 0; j < faceVertexCnt; j++) {
				glVertex3f(V[vList[index]].x, V[vList[index]].y, V[vList[index]].z);
				if (hasVT) {
					glTexCoord2f(VT[vtList[index]].x, VT[vtList[index]].y);
				}
				if (hasVN) {
					glNormal3f(VN[vnList[index]].x, VN[vnList[index]].y, VN[vnList[index]].z);
				}
				index++;
			}
			glEnd();
		}
	}
}
