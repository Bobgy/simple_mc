#ifndef _AUXILIARY_
#define _AUXILIARY_

#include "vec.h"
#include "stdafx.h"

const float golden[] = { 0.85f, 0.65f, 0.2f, 1.0f };
const float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const float black[] = { 0, 0, 0, 1 };
const float light_grey[] = { 0.75, 0.75, 0.75, 1 };
const float grey[] = { 0.5, 0.5, 0.5, 1 };
const float dark_grey[] = { 0.1, 0.1, 0.1, 1 };
const float sun[] = { 0.8, 0.7, 0.6, 1 };
void use_material(const GLfloat *d, const GLfloat *s, const GLfloat *e, int shine);

#endif
