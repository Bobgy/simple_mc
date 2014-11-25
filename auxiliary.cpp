#include "auxiliary.h"
#include <stdlib.h>
#include <GL/glut.h>

void use_material(GLfloat *d, GLfloat *s, GLfloat *e, int shine)
{
	if(d!=NULL)glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, d);
    if(s!=NULL)glMaterialfv(GL_FRONT, GL_SPECULAR, s);
    if(e!=NULL)glMaterialfv(GL_FRONT, GL_EMISSION, e);
    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,shine);
}
