#include <stdlib.h>
#include "KeyBoardControl.h"
#include "world.h"
#include "entity.h"
#include "Render.h"

KeyboardControl keyboard;
extern void regenTableList(GLint);
extern bool bPersp, bWire;
extern int tableList, wWidth, wHeight;
extern GLuint tex;
extern void update_center();
extern void updateView(int, int);
extern World world;
extern Entity observer;
extern Render render;
bool bDebugDepthMap = false;

void KeyDown(unsigned char key, int x, int y)
{
	keyboard.key_state[key]=true;
	switch (key)
	{
		case 27:
		case 'q': { exit(0); break; }
		case 'p': { bPersp = !bPersp; break; }
		case 'o': { bWire = !bWire; break; }
		case 't': {
			tex = (tex + 1) % 7;
			regenTableList(tableList);
			break;
		}
		case '/': {
			//write world to file
			break;
		}
	}
	update_center();
}
void KeyUp(unsigned char key, int x, int y)
{
	keyboard.key_state[key]=false;
}
void SpecialKeyDown(int key, int x, int y)
{
	static bool fullscreen = 0;
	switch (key){
		case GLUT_KEY_F11:
			if (fullscreen){
				glutReshapeWindow(1000, 700);
				glutPositionWindow(0, 0);
				fullscreen = 0;
			} else {
				glutFullScreen();
				fullscreen = 1;
			}
			break;
		case GLUT_KEY_F10:
			bDebugDepthMap ^= 1;
			break;
	}
}
bool KeyboardControl::get_state(unsigned char key)
{
	return keyboard.key_state[key];
}
void KeyboardControl::init(){
	glutKeyboardUpFunc(KeyUp);
	glutKeyboardFunc(KeyDown);
	glutSpecialFunc(SpecialKeyDown);
}