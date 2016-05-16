#ifndef _AUXILIARY_
#define _AUXILIARY_

#include <core/vec.h>
#include <stdafx.h>

const float golden[] = { 0.9f, 0.9f, 0.2f, 1.0f };
const float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const float black[] = { 0, 0, 0, 1 };
const float light_grey[] = { 0.75, 0.75, 0.75, 1 };
const float grey[] = { 0.5, 0.5, 0.5, 1 };
const float dark_grey[] = { 0.1, 0.1, 0.1, 1 };
const float sun[] = { 0.8, 0.7, 0.6, 1 };
const float green[] = { 0.2, 1.0, 0.5, 1 };

//use material ambient, diffusion, specular, emission and shininess
void use_material(const GLfloat *a, const GLfloat *d, const GLfloat *s, const GLfloat *e, int shine);

class CubeTexCoord{

private:

	flt hi, wi;
	flt c[6][4][2];
	void set(flt c[2], int a, int b){
		c[0] = b * wi;
		c[1] = a * hi;
	}


public:

	CubeTexCoord(Vec3i sz, int s, int t, int h, int w);

	//get the coordinate for face f vertex v
	const GLfloat *getCoord(int f, int v) const {
		return c[f][v];
	}
};

#endif
