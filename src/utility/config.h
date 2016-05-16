#ifndef _CONFIG_H_
#define _CONFIG_H_

extern int view_mode;

extern flt range_touch;

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

//current log level, logs with a larger level will be outputed
extern int log_level;
#endif