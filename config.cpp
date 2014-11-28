#include "Render.h"
#include "vec.h"

int view_mode = VIEW_MODE_FIRST_PERSON;

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
