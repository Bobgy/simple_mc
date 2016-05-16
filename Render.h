#ifndef _RENDER_H
#define _RENDER_H

#include <stdlib.h>
#include "stdafx.h"
#include <cstdio>
#include "entity.h"
#include "auxiliary.h"
#include <vector>
#define BITMAP_ID 0x4D42

#define VIEW_MODE_FIRST_PERSON 0
#define VIEW_MODE_THIRD_PERSON_BACK 1
#define VIEW_MODE_THIRD_PERSON_FRONT 2
#define VIEW_MODE_TOTAL_NUMBER 3

#define VIEW_DISTANCE 80.0f

class View;

class Render{
public:
	GLuint texture[100];
	//the position of eye and view center
	Vec3f center, eye;

	void init();
	void setupFBO();

	//render a cube with 6 faces encoded by state
	void renderCube(int type, int state);

	//render a cube using tex
	void renderCubeTex(int type, const CubeTexCoord &tex);

	void renderScene();

	void texLoad(int i, const char *filename);

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

	//render the player at position "observer" with radius r and height h
	void renderPlayer(Entity observer, flt r, flt h);

	//update the center and eye vector
	void update_center(View &cursor);

	//setup perspective matrices
	//lightSource toggles the setting
	//parallel toggles orthogonal or perspective
	void setupPerspective(const Vec3f eye, Vec3f center, Vec3f upVector, bool lightSource, bool parallel);

	//draw an OBJ item
	void draw_item();

private:

	//Whether texture is turned on
	bool bTexture;

	unsigned char *LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
	unsigned char *LoadPngImage(const char *filename);
	void set(int k);
	unsigned char tex_check[256][256][3];
};

void renderSeenBlock(BlockAndFace seen_block);

GLint genTableList();

void regenTableList(GLint lid);

void renderInfo(Entity);

void renderTableList();

void renderCross();

void renderGUI(Entity);

void renderSceneDynamic(bool bObserver, Entity observer);

void display();

#endif