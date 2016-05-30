#include "stdafx.h"

#include <random>

#include "render/render.h"
#include "utility/vec.h"

int view_mode = VIEW_MODE_FIRST_PERSON;
flt range_touch = 1;

//the eye height for the observer
flt h_eye = 0.8;

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
bool bCreative = false;

//whether gravity works on player
bool bGravity = false;

//whether sun light is moving
bool bMovingLight = false;

//the size of the shadow map and the function to update shadow_map_size
double shadow_map_ratio = 2.0;
int SHADOW_MAP_WIDTH = 512 * shadow_map_ratio;
int SHADOW_MAP_HEIGHT = 1024 * shadow_map_ratio;
void update_shadow_map_size(){
	SHADOW_MAP_WIDTH = 512 * shadow_map_ratio;
	SHADOW_MAP_HEIGHT = 1024 * shadow_map_ratio;
}

//scene tableListID
GLint tableList;

//desired frame rate
flt FRAME_RATE = 60.0f;

bool bDebugDepthMap = false;
bool bWire = false;

random_device k_random_device;
mt19937 k_pseudo_gen(k_random_device());

bool bDisplay = true;
bool bSimpleCube = false;

flt k_ai_speed = 10.0f;
flt k_float_speed = 10.0f;
flt k_collision_force = 160000.f;
flt k_steering_force = 40000.f;
flt k_avoidance_radius_ratio = 2.0f;
flt k_nav_force_deteriorate_rate = 0.4f;