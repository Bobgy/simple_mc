#include"KeyBoardControl.h"
#include "glut.h"

KB_control keyboard;
extern int GenTableList();
extern bool bPersp, bWire;
extern int tableList, tex, wWidth, wHeight;
extern void update_center();
extern void updateView(int, int);

void KeyDown(unsigned char key, int x, int y)
{
	keyboard.key_state[key]=true;
	switch (key)
	{
	case 27:
	case 'q': {exit(0); break; }
	case 'p': {bPersp = !bPersp; break; }
	case 'o': {bWire = !bWire; break; }
	case 't': {
		tex = (tex + 1) % 9;
		tableList = GenTableList();
		break;
	}
	}

	update_center();
	updateView(wWidth, wHeight);
}
void KeyUp(unsigned char key, int x, int y)
{
	keyboard.key_state[key]=false;
}
bool  KB_control::get_state(unsigned char key)
{
	return keyboard.key_state[key];
}
void KB_control::init(){
	glutKeyboardUpFunc(KeyUp);
	glutKeyboardFunc(KeyDown);
}