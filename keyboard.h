#ifndef _KEY_BOARD_CONTROL_H
#define _KEY_BOARD_CONTROL_H

#include <cstring>

extern bool bDebugDepthMap;

class KeyboardControl{
private:
	bool key_state[512];
	bool special_key_state[512];

public:
	KeyboardControl(){
		memset(key_state, 0, sizeof key_state);
		memset(special_key_state, 0, sizeof special_key_state);
	}
	~KeyboardControl(){}
	bool is_walking() const {
		return (key_state['w'] ^ key_state['s']) || (key_state['a'] ^ key_state['d']);
	}
	friend void KeyDown(unsigned char key, int x, int y);
	friend void KeyUp(unsigned char key, int x, int y);
	friend void SpecialKeyUp(int key, int x, int y);
	friend void SpecialKeyDown(int key, int x, int y);
	void init();
	bool get_special_state(int key) const {
		return special_key_state[key];
	}
	bool get_state(unsigned char key) const {
		return key_state[key];
	}
};
 
#endif
