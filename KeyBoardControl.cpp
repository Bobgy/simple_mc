#include <stdlib.h>
#include "KeyBoardControl.h"
#include "world.h"
#include "entity.h"

KeyboardControl keyboard;
extern void RegenTableList(GLint);
extern bool bPersp, bWire;
extern int tableList, tex, wWidth, wHeight;
extern void update_center();
extern void updateView(int, int);
extern World world;
extern Entity observer;

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
			RegenTableList(tableList);
			break;
		}
	}
	update_center();
}
void KeyUp(unsigned char key, int x, int y)
{
	keyboard.key_state[key]=false;
}
bool KeyboardControl::get_state(unsigned char key)
{
	return keyboard.key_state[key];
}
void KeyboardControl::init(){
	glutKeyboardUpFunc(KeyUp);
	glutKeyboardFunc(KeyDown);
}