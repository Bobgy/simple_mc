#include "stdafx.h"

#include "utility/keyboard.h"
#include "utility/screenshot.h"
#include "utility/config.h"
#include "utility/view.h"

#include "game/world.h"
#include "game/entity.h"
#include "game/block.h"
#include "game/game.h"
#include "game/event_manager.h"

#include "render/render.h"

extern Render render;
extern block_type type;
extern int screenshot_count;

void KeyDown(unsigned char key, int x, int y)
{
	if (key >= 'A' && key <= 'Z')key += 'a' - 'A';
	Keyboard *keyboard = CurrentGame()->getKeyboard();
	keyboard->key_state[key] = true;
	keyboard->m_key_event_board.onEvent(key, EnumEventType::ON_DOWN);

	// HACK
	switch (key)
	{
		case 27:
		case 'q': { exit(0); break; }
		case 'o': { bWire = !bWire; break; }
		case '/': {
			//write world to file
			CurrentGame()->getWorld()->print();
			break;
		}
		default:
		{
			if ((key >= '0') && (key <= '9'))
			{
				int k = key - '0';
				type = block_type(k);
			}
		}
	}
}
void KeyUp(unsigned char key, int x, int y)
{
	if (key >= 'A' && key <= 'Z')key += 'a' - 'A';
	Keyboard *keyboard = CurrentGame()->getKeyboard();
	keyboard->key_state[key] = false;
	keyboard->m_key_event_board.onEvent(key, EnumEventType::ON_UP);
}
void SpecialKeyUp(int key, int x, int y){
	Keyboard *keyboard = CurrentGame()->getKeyboard();
	keyboard->special_key_state[key] = false;
	keyboard->m_special_key_event_board.onEvent(key, EnumEventType::ON_UP);
}
void SpecialKeyDown(int key, int x, int y)
{
	Keyboard *keyboard = CurrentGame()->getKeyboard();
	keyboard->special_key_state[key] = true;
	keyboard->m_special_key_event_board.onEvent(key, EnumEventType::ON_DOWN);

	// HACK
	static bool fullscreen = 0;
	switch (key){

		//toggle Creative Mode
		case GLUT_KEY_F2:
			bCreative ^= 1;
			bGravity ^= 1;
			break;
		/*
		//toggle the shader
		case GLUT_KEY_F1:
			bCustomGLSL ^= 1;
			break;
		*/

		//toggle view_mode
		case GLUT_KEY_F5:
			view_mode = (view_mode + 1) % VIEW_MODE_TOTAL_NUMBER;
			break;

		//toggle showing the observer
		case GLUT_KEY_F6: {
			Entity *entity = CurrentGame()->getPlayerEntity();
			if (entity != nullptr) entity->render_config.is_visible ^= 1;
			break;
		}

		//toggle moving sun light
		case GLUT_KEY_F7:
			bMovingLight ^= 1;
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

void Keyboard::setup(){
	glutKeyboardUpFunc(KeyUp);
	glutKeyboardFunc(KeyDown);
	glutSpecialFunc(SpecialKeyDown);
	glutSpecialUpFunc(SpecialKeyUp);
}
