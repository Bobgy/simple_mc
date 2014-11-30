#include "stdafx.h"
#include "auxiliary.h"

void use_material(const GLfloat *a, const GLfloat *d, const GLfloat *s, const GLfloat *e, int shine)
{
	if(a!=NULL)glMaterialfv(GL_FRONT, GL_AMBIENT, a);
	if(d!=NULL)glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
    if(s!=NULL)glMaterialfv(GL_FRONT, GL_SPECULAR, s);
    if(e!=NULL)glMaterialfv(GL_FRONT, GL_EMISSION, e);
    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,shine);
}