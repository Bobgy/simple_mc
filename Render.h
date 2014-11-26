#ifndef _RENDER_H
#define _RENDER_H

#include <stdlib.h>
#include "stdafx.h"
//#include <windows.h>
#include <cstdio>
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


#endif
