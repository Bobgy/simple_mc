#ifndef _STDAFX_H_
#define _STDAFX_H_

#include <cstdlib>
#include <memory>
#include <functional>


#include <GL/glew.h>
#include <GL/freeglut.h>

#include "utility/logger.h"
#include "utility/config.h"

#include "utility/string_id.h"
#include "utility/vec.h"

#include "game/game.h"

#define RETURN_IF_NULL(obj) if ((obj) == nullptr) return

using namespace std;

#endif