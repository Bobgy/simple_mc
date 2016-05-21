#include "render/render.h"
#include "utility/vec.h"

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
int texPlayer = 19;

//toggles whether we use the custom GLSL that supports shadow
bool bCustomGLSL = true;

//whether the player is in creative mode
bool bCreative = true;

//whether gravity works on player
bool bGravity = false;

//whether sun light is moving
bool bMovingLight = false;

//the size of the shadow map and the function to update shadow_map_size
double shadow_map_ratio = 2.0;
int SHADOW_MAP_WIDTH = 1024 * shadow_map_ratio;
int SHADOW_MAP_HEIGHT = 1024 * shadow_map_ratio;
void update_shadow_map_size(){
	SHADOW_MAP_WIDTH = 1024 * shadow_map_ratio;
	SHADOW_MAP_HEIGHT = 1024 * shadow_map_ratio;
}

//scene tableListID
GLint tableList;

//desired frame rate
flt FRAME_RATE = 60.0f;

//current log level, logs with a larger level will be outputed
int log_level = 0;
