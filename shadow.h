#ifndef _SHADOW_H_
#define _SHADOW_H_

#define SHADOW_MAP_RATIO 8.0
const int SHADOW_MAP_WIDTH = 1024 * SHADOW_MAP_RATIO;
const int SHADOW_MAP_HEIGHT = 1024 * SHADOW_MAP_RATIO;

void generateShadowFBO();
void setTextureMatrix(void);

#endif