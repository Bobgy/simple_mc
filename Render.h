#ifndef _RENDER_H
#define _RENDER_H

#include <stdlib.h>
#include "stdafx.h"
#include <cstdio>
#include "entity.h"
#define BITMAP_ID 0x4D42

class Render{
public:
	void init();
	void setupFBO();
	void draw_Cube(int type,int state);
private:
	void texload(int i,const char *filename);
	unsigned char *LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
	void set(int k);
	GLuint texture[10];
	unsigned char tex[256][256][3];
};

//Draw the observer at position given by Entity observer, with radiu r
void DrawObserver(Entity observer, flt r, flt h);

void DrawSeenBlock(block_and_face seen_block);

void Draw_Scene();

GLint genTableList();

void regenTableList(GLint lid);

void drawInfo(Entity);

void drawTableList();

void drawCross();

void drawGUI(Entity);

#endif
