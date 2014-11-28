#ifndef _CONFIG_H_
#define _CONFIG_H_

extern int view_mode;

//the collision radius for the observer
extern flt r;

//the height for the observer
extern flt h;

//the eye height for the observer
extern flt h_eye;

//toggles whether the observer is drawn
extern bool bObserver;

//the distance between observer and camera in third person views
extern flt observer_dist;

//the smoothness of the ground, v' = v * smoothness
extern flt smoothness_ground;

//whether show all the lines of boxes for debug
extern bool bBoxLine;

#endif