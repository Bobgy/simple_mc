#ifndef _SHADOW_H_
#define _SHADOW_H_

#define SHADOW_MAP_RATIO 4.0
const int SHADOW_MAP_WIDTH = 1024 * SHADOW_MAP_RATIO;
const int SHADOW_MAP_HEIGHT = 724 * SHADOW_MAP_RATIO;

void generateShadowFBO();
void setupMatrices(const flt lightDir[4], Vec3f center, bool lightSource = false);
void setTextureMatrix(void);

#endif