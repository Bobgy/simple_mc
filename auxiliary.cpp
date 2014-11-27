#include "stdafx.h"
#include "auxiliary.h"

void use_material(const GLfloat *d, const GLfloat *s, const GLfloat *e, int shine)
{
	if(d!=NULL)glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, d);
    if(s!=NULL)glMaterialfv(GL_FRONT, GL_SPECULAR, s);
    if(e!=NULL)glMaterialfv(GL_FRONT, GL_EMISSION, e);
    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,shine);
}
