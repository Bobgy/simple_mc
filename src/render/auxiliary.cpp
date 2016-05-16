#include <stdafx.h>
#include <render/auxiliary.h>

void use_material(const GLfloat *a, const GLfloat *d, const GLfloat *s, const GLfloat *e, int shine)
{
	if(a!=NULL)glMaterialfv(GL_FRONT, GL_AMBIENT, a);
	if(d!=NULL)glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
    if(s!=NULL)glMaterialfv(GL_FRONT, GL_SPECULAR, s);
    if(e!=NULL)glMaterialfv(GL_FRONT, GL_EMISSION, e);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}

CubeTexCoord::CubeTexCoord(Vec3i sz, int s, int t, int h, int w){
	hi = 1.0 / h;
	wi = 1.0 / w;
	{ //back
		flt(&C)[4][2] = c[3];
		int s0 = s, t0 = t + sz[2] + sz[0];
		set(C[0], s0, t0);
		set(C[1], s0 + sz[1], t0);
		set(C[2], s0 + sz[1], t0 + sz[2]);
		set(C[3], s0, t0 + sz[2]);
	}
	{ //left
		flt(&C)[4][2] = c[5];
		int s0 = s, t0 = t + sz[2];
		set(C[0], s0, t0);
		set(C[1], s0 + sz[1], t0);
		set(C[2], s0 + sz[1], t0 + sz[0]);
		set(C[3], s0, t0 + sz[0]);
	}
	{ //right
		flt(&C)[4][2] = c[4];
		int s0 = s, t0 = t - sz[0];
		set(C[0], s0, t0);
		set(C[1], s0 + sz[1], t0);
		set(C[2], s0 + sz[1], t0 + sz[0]);
		set(C[3], s0, t0 + sz[0]);
	}
	{ //top
		flt(&C)[4][2] = c[0];
		int s0 = s - sz[0], t0 = t;
		set(C[0], s0, t0);
		set(C[1], s0 + sz[0], t0);
		set(C[2], s0 + sz[0], t0 + sz[2]);
		set(C[3], s0, t0 + sz[2]);
	}
	{ //front
		flt(&C)[4][2] = c[2];
		set(C[0], s + sz[1], t + sz[2]);
		set(C[1], s, t + sz[2]);
		set(C[2], s, t);
		set(C[3], s + sz[1], t);
	}
	{ //bottom
		flt(&C)[4][2] = c[1];
		int s0 = s - sz[0], t0 = t + sz[2];
		set(C[0], s0, t0);
		set(C[1], s0 + sz[0], t0);
		set(C[2], s0 + sz[0], t0 + sz[2]);
		set(C[3], s0, t0 + sz[2]);
	}
}