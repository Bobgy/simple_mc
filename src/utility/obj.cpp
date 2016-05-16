#include "obj.h"
#include <stdafx.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <render/auxiliary.h>

using namespace std;

void ObjModel::parse(face f)
{
	string s = f[0];
	if (s[0] == '=') return;
	int faceVertexCnt = f.size();
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
		for (int k = 0; k < faceVertexCnt; k++){
			s = f[k];
			sscanf_s(s.c_str(), "%d", &indexV);
			vList.push_back(indexV - 1);
		}
	}
	else if (slashCnt == 1) {
		for (int k = 0; k < faceVertexCnt; k++){
			s = f[k];
			sscanf_s(s.c_str(), "%d/%d", &indexV, &indexVT);
			vList.push_back(indexV - 1);
			vtList.push_back(indexVT - 1);
		}
	}
	else if (slashCnt == 2) {
		if (sp1 + 1 == sp2) { // no vt
			for (int k = 0; k < faceVertexCnt; k++){
				s = f[k];
				sscanf_s(s.c_str(), "%d//%d", &indexV, &indexVN);
				vList.push_back(indexV - 1);
				vnList.push_back(indexVN - 1);
			}
		}
		else { // has vt
			for (int k = 0; k < faceVertexCnt; k++){
				s = f[k];
				sscanf_s(s.c_str(), "%d/%d/%d", &indexV, &indexVT, &indexVN);
				vList.push_back(indexV - 1);
				vtList.push_back(indexVT - 1);
				vnList.push_back(indexVN - 1);
			}
		}
	}
	else {
		cerr << "Error while parsing slash of faces" << endl;
	}
	
	if (slashCnt < 2) {
		vertex3f ta, tb, tx;
		for (int k = vList.size() - faceVertexCnt; k < vList.size(); k++) {
			assert(k >= 0);
			int index1 = vList[(k == vList.size() - faceVertexCnt) ? (vList.size()-1) : (k - 1)];
			int index2 = vList[(k + 1 == vList.size()) ? (vList.size() - faceVertexCnt) : (k + 1)];
			int index3 = vList[k];
			Vec3f A, B, X;
			assert(index1 >= 0 && index1 < V.size());
			assert(index2 >= 0 && index2 < V.size());
			ta = V[index1]; tb = V[index2];
			//cerr << k << ' ' << vList.size() << endl;
			A[0] = ta.x - V[index3].x; A[1] = ta.y - V[index3].y; A[2] = ta.z - V[index3].z;
			B[0] = tb.x - V[index3].x; B[1] = tb.y - V[index3].y; B[2] = tb.z - V[index3].z;
			X = (B^A).normalize();
			tx.x = X[0]; tx.y = X[1]; tx.z = X[2];
			//tx.x = 1;	tx.y = tx.z = 0;
			VN.push_back(tx);
			vnList.push_back(VN.size() - 1);
		}
	}
}

void ObjModel::clear()
{
	V.clear(); VN.clear(); VT.clear(); 
	vList.clear(); vtList.clear(); vnList.clear(); 
	F.clear();
	mtlTable.clear();
}

void ObjModel::loadMTL(string filename)
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

void ObjModel::read(string filename)
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
			//v.x = v.y = v.z = 1;
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
			for (int k = t.size() - 1; k >= 0 && (t[k] != '\\' && t[k] != '/'); k--)
				t.erase(t.end() - 1, t.end());
			loadMTL(t + s);
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
	for (int i = 0; i < F.size(); i++) {
		parse(F[i]);
	}
	cerr << "Read from " << filename << endl;
	display_list_id = glGenLists(1);
	if (display_list_id == 0) {
		cerr << "Generating display list failed!" << endl;
		return;
	}
	glNewList(display_list_id, GL_COMPILE);
	draw();
	glEndList();
}

void ObjModel::draw()
{
	//glDisable(GL_CULL_FACE);
	int index = 0;
	bool hasVT = vtList.size();
	for (int i = 0; i < F.size(); i++) {
		if (F[i][0][0] == '=') {
			string s = F[i][0];
			s.erase(0, 1);
			mtl data = mtlTable.find(s)->second;
			use_material(data.Ka, data.Kd, data.illum == 2 ? data.Ks : black, NULL, data.Ns);
			continue;
		}
		int faceVertexCnt = F[i].size();
		glBegin(GL_POLYGON);
		for (int j = 0; j < faceVertexCnt; j++) {
			if (hasVT) glTexCoord2f(VT[vtList[index]].x, VT[vtList[index]].y);
			glNormal3f(VN[vnList[index]].x, VN[vnList[index]].y, VN[vnList[index]].z);
			glVertex3f(V[vList[index]].x, V[vList[index]].y, V[vList[index]].z);
			index++;
		}
		glEnd();
	}
}
