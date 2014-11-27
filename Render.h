#ifndef _RENDER_H
#define _RENDER_H

#include <stdlib.h>
#include "stdafx.h"
#include <cstdio>
#include "entity.h"
#include <vector>
#define BITMAP_ID 0x4D42

class Render{
public:
	GLuint texture[10];

	void init();
	void setupFBO();
	void renderCube(int type,int state);
	void renderScene();
	void beginTranslate(Vec3f p);
	void endTranslate();
	void setTextureState(bool);
	void texLoadPNG(int i, const char *filename);

private:
	
	bool bTexture;
	void texload(int i,const char *filename);
	unsigned char *LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
	unsigned char *LoadPngImage(const char *filename);
	void set(int k);
	unsigned char tex_check[256][256][3];
};

//render the observer at position given by Entity observer, with radiu r
void renderObserver(Entity observer, flt r, flt h);

void renderSeenBlock(block_and_face seen_block);

GLint genTableList();

void regenTableList(GLint lid);

void renderInfo(Entity);

void renderTableList();

void renderCross();

void renderGUI(Entity);

#endif
