#ifndef _KEY_BOARD_CONTROL_H
#define _KEY_BOARD_CONTROL_H

class KeyboardControl{
private:
	bool key_state[2000];

public:
	KeyboardControl(){
		for(int i=0;i<2000;i++) key_state[i]=false;
	}
	~KeyboardControl(){}
	bool get_state(unsigned char key);
	friend void KeyDown(unsigned char key, int x, int y);
	friend void KeyUp(unsigned char key, int x, int y);
	void init();
};
 
#endif
