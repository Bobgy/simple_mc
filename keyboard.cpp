#include <stdlib.h>
#include "keyboard.h"
#include "world.h"
#include "entity.h"
#include "render.h"
#include "screenshot.h"
#include "config.h"
#include "cursor.h"

KeyboardControl keyboard;
extern void regenTableList(GLint);
extern int tableList, wWidth, wHeight;
extern GLuint tex;
extern void updateView(int, int);
extern World world;
extern Entity observer;
extern Render render;
extern Cursor cursor;
extern block_type type;
extern block_type int2block_type[10];
extern int screenshot_count;
bool bDebugDepthMap = false, bWire = false;

void KeyDown(unsigned char key, int x, int y)
{
	keyboard.key_state[key]=true;
	switch (key)
	{
		case 27:
		case 'q': { exit(0); break; }
		case 'o': { bWire = !bWire; break; }
		case 't': {
			tex =1+ (tex + 1) % 7;
			regenTableList(tableList);
			break;
		}
		case '/': {
			//write world to file
			world.print();
			break;
		}
		default:
		{
			if ((key >= '0') && (key <= '9'))
			{
				int k = key - '0';
				type = int2block_type[k];
			//	cout << key << " " << k << " " << type << endl;
			}
		}
	}
	render.update_center(cursor);
}
void KeyUp(unsigned char key, int x, int y)
{
	keyboard.key_state[key]=false;
}
void SpecialKeyDown(int key, int x, int y)
{
	static bool fullscreen = 0;
	switch (key){
		
		//toggle the shader
		case GLUT_KEY_F1:
			bCustomGLSL ^= 1;
			break;

		//toggle view_mode
		case GLUT_KEY_F5:
			view_mode = (view_mode + 1) % VIEW_MODE_TOTAL_NUMBER;
			break;

		//toggle showing the observer
		case GLUT_KEY_F6:
			bObserver ^= 1;
			break;
		//toggle showing the box lines
		case GLUT_KEY_F9:
			bBoxLine ^= 1;
			break;

		//toggle showing the shadow depth map at the top right corner
		case GLUT_KEY_F10:
			bDebugDepthMap ^= 1;
			break;

		//toggle fullscreen
		case GLUT_KEY_F11:
			if (fullscreen){
				glutReshapeWindow(1000, 700);
				glutPositionWindow(0, 0);
				fullscreen = 0;
			}
			else {
				glutFullScreen();
				fullscreen = 1;
			}
			break;

		//screen_shot
		case GLUT_KEY_F12: {
			std::string filename = "screenshot_" + std::to_string(screenshot_count) + ".bmp";
			ScreenShot(filename);
			screenshot_count++;
			break;
		}
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