	#include"KeyBoardControl.h"

	void KB_control::KeyDown(unsigned char key, int x, int y)
	{
		key_state[key]=true;
	}
	void KB_control::KeyUp(unsigned char key, int x, int y)
	{
		key_state[key]=false;
	}
	bool  KB_control::get_state(unsigned char key)
	{
		return key_state[key];
	}
