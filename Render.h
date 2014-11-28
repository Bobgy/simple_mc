#ifndef _RENDER_H
#define _RENDER_H

#include <stdlib.h>
#include "stdafx.h"
#include <cstdio>
#include "entity.h"
#include <vector>
#define BITMAP_ID 0x4D42

#define VIEW_MODE_FIRST_PERSON 0
#define VIEW_MODE_THIRD_PERSON_BACK 1
#define VIEW_MODE_THIRD_PERSON_FRONT 2
#define VIEW_MODE_TOTAL_NUMBER 3

class Cursor;

class Render{
public:
	GLuint texture[10];
	//the position of eye and view center
	Vec3f center, eye;

	void init();
	void setupFBO();
	void renderCube(int type,int state);
	void renderScene();
	void texLoadPNG(int i, const char *filename);
	
	//set whether we use texture or not
	void setTextureState(bool);

	//render box lines around the player for debug use
	void Render::renderBoxLine();

	//use this function to start a series of transformation
	void beginTransform();

	//use this function to end a series of transformation
	void endTransform();

	//load in the identity matrix
	void loadIdentity();

	//rotate the following objects angle degree along the axis
	//need to be inside beginTransform() and endTransform
	void rotate(flt angle, Vec3f axis);

	//translate the following objects along vector p
	//need to be inside beginTransform() and endTransform
	void translate(Vec3f p);

	//scale the following objects each by vector p
	//need to be inside beginTransform() and endTransform
	void scale(Vec3f p);

	//render the observer at position observer with radius r and height h
	void renderObserver(Entity observer, flt r, flt h);

	//update the center and eye vector
	void update_center(Cursor &cursor);

private:

	//Whether texture is turned on
	bool bTexture;

	void texload(int i,const char *filename);
	unsigned char *LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
	unsigned char *LoadPngImage(const char *filename);
	void set(int k);
	unsigned char tex_check[256][256][3];
};

void renderSeenBlock(block_and_face seen_block);

GLint genTableList();

void regenTableList(GLint lid);

void renderInfo(Entity);

void renderTableList();

void renderCross();

void renderGUI(Entity);

void renderSceneDynamic(bool bObserver, Entity observer);

void DisplayScene();

#endif
