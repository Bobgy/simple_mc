#include "obj.h"
#include "stdafx.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "auxiliary.h"

using namespace std;

void objModel::parse()
{
	for (int i = 0; i < F.size(); i++) {
		if (F[i][0][0] == '=') continue;
		face f = F[i];
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
	V.clear(); VN.clear(); VT.clear(); 
	vList.clear(); vtList.clear(); vnList.clear(); 
	F.clear();
	mtlTable.clear();
}

void objModel::loadMTL(string filename)
{
	ifstream fin(filename);
	if (!fin) {
		cerr << "Error : No such MTL file as " << filename << endl;
		return;
	}
	else {
		cerr << "Loaded " << filename << endl;
	}
	string s;
	while (fin >> s) {
		while (s == "newmtl") {
			string mtlName;
			fin >> mtlName;
			mtl data;
			while (fin >> s) {
				if (s == "newmtl") break;
				else if (s == "Ka") {
					fin >> data.Ka[0] >> data.Ka[1] >> data.Ka[2];
					data.KaSet = true;
					getline(fin, s);
				}
				else if (s == "Kd") {
					fin >> data.Kd[0] >> data.Kd[1] >> data.Kd[2];
					if (!data.KaSet) {
						memcpy(data.Ka, data.Kd, sizeof(data.Kd));
					}
					getline(fin, s);
				}
				else if (s == "Ks") {
					fin >> data.Ks[0] >> data.Ks[1] >> data.Ks[2];
					getline(fin, s);
				}
				else if (s == "illum") {
					fin >> data.illum;
				}
				else if (s == "d" || s == "Tr") {
					fin >> data.Tr;
					data.Ks[3] = data.Ka[3] = data.Kd[3] = data.Tr;
				}
				else if (s == "Ns") {
					fin >> data.Ns;
				}
				else {
					cerr << "Something wrong with MTL" << endl;
				}
			}
			mtlTable.insert(make_pair(mtlName, data));
		}
		getline(fin, s);
	}
}

void objModel::read(string filename)
{
	string s;
	ifstream fin(filename);
	if (!fin) {
		cerr << "Error : No such OBJ file as " << filename << endl;
		return;
	}
	clear();
	while (fin >> s) {
		if (s == "v") {
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
		else if (s == "mtllib") {
			fin >> s;
			string t = filename;
			for (int k = t.size() - 1; k>=0 && (t[k] != '\\' && t[k] != '/'); k--)
				t.erase(t.end()-1,t.end());
			loadMTL(t+s);
		}
		else if (s == "usemtl") {
			fin >> s;
			s = '=' + s;
			face f;
			f.push_back(s);
			F.push_back(f);
		}
		else {
			getline(fin, s);
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
		if (F[i][0][0] == '=') {
			string s = F[i][0];
			s.erase(0, 1);
			//cerr << s << endl;
			mtl data = mtlTable.find(s)->second;
			use_material(data.Ka, data.Kd, data.illum==2?data.Ks:black, NULL, data.Ns);
			continue;
		}
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
