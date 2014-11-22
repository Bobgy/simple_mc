#ifndef _KEYBOARDCONTROL_H
#define _KEYBOARDCONTROL_H

class KB_control{
private:
	bool key_state[2000];

public:
	KB_control()
	{
		for(int i=0;i<2000;i++) key_state[i]=false;
	}
	~KB_control()
	{

	}
	void KeyDown(unsigned char key, int x, int y);
	void KeyUp(unsigned char key, int x, int y);
	bool get_state(unsigned char key);
};
 
#endif
