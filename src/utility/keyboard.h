#ifndef _KEY_BOARD_CONTROL_H
#define _KEY_BOARD_CONTROL_H

#include "stdafx.h"

#include <cstring>

#include "game/event_manager.h"

extern bool bDebugDepthMap;

class Keyboard{
// private members
private:
	bool key_state[256];
	bool special_key_state[512];

// public members
public:
	EventBoard<unsigned char> m_key_event_manager;
	EventBoard<int> m_special_key_event_manager;

// public methods
public:
	Keyboard(){
		memset(key_state, 0, sizeof key_state);
		memset(special_key_state, 0, sizeof special_key_state);
	}
	~Keyboard(){}
	bool is_walking() const {
		return (key_state['w'] ^ key_state['s']) || (key_state['a'] ^ key_state['d']);
	}
	friend void KeyDown(unsigned char key, int x, int y);
	friend void KeyUp(unsigned char key, int x, int y);
	friend void SpecialKeyUp(int key, int x, int y);
	friend void SpecialKeyDown(int key, int x, int y);
	void setup();
	bool get_special_state(int key) const {
		return special_key_state[key];
	}
	bool get_state(unsigned char key) const {
		return key_state[key];
	}
};
 
#endif
