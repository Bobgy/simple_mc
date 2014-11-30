#include "Render.h"
#include "vec.h"

int view_mode = VIEW_MODE_FIRST_PERSON;
flt range_touch = 1;

//the collision radius for the observer
flt r = 0.45;

//the height for the observer
flt h = 1.6;

//the eye height for the observer
flt h_eye = h * 0.8;

//toggles whether the observer is drawn
bool bObserver = false;

//the distance between observer and camera in third person views
flt observer_dist = 4.0;

//the smoothness of the ground, v' = v * smoothness
flt smoothness_ground = 0.80;

//whether show all the lines of boxes for debug
bool bBoxLine = false;

//whether show the collision box for entities
bool bCollisionBox = false;

//the texture id for parts of the player
//in order head, body, leg, arm
int texPlayer[] = 8;

//toggles whether we use the custom GLSL that supports shadow
bool bCustomGLSL = true;