#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <random>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "utility/vec.h"

//#define MAP_TUNNEL

extern int view_mode;

extern flt range_touch;

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

//whether show the collision box for entities
extern bool bCollisionBox;

//the texture id for the body of observer
extern int texPlayer;

//toggles whether we use the custom GLSL that supports shadow
extern bool bCustomGLSL;

//whether the player is in creative mode
extern bool bCreative;

//whether gravity works on player
extern bool bGravity;

//whether sun light is moving
extern bool bMovingLight;

//the size of the shadow map and the function to update shadow_map_size
extern double shadow_map_ratio;
extern int SHADOW_MAP_WIDTH;
extern int SHADOW_MAP_HEIGHT;
void update_shadow_map_size();

//scene tableListID
extern GLint tableList;

//desired frame rate
extern flt FRAME_RATE;
extern flt GAME_SPEED;

extern bool bDebugDepthMap;
extern bool bWire;

extern random_device k_random_device;
extern mt19937 k_pseudo_gen;

extern bool bDisplay;
extern bool bSimpleCube;

extern flt k_ai_speed;
extern flt k_float_speed;
extern flt k_collision_force;
extern flt k_steering_force;
extern flt k_avoidance_radius_ratio;
extern flt k_nav_force_deteriorate_rate;
extern size_t k_temporary_priority_last_ticks;

extern bool bPriorityEnabled;

extern int32_t k_map_size;
#endif