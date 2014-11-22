#ifndef KEYBOARDCONTROL_H
#define KEYBOARDCONTROL_H

class KB_control{
private:
	bool key_state[2000];

public:
	KB_control(){
		for(int i=0;i<2000;i++) key_state[i]=false;
	}
	~KB_control(){}
	bool get_state(unsigned char key);
	friend void KeyDown(unsigned char key, int x, int y);
	friend void KeyUp(unsigned char key, int x, int y);
	void init();
};
 
#endif
