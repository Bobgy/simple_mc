#ifndef _STDAFX_H_
#define _STDAFX_H_

#include <cstdlib>
#include <memory>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "utility/vec.h"
#include "utility/config.h"
#include "utility/logger.h"

#define RETURN_IF_NULL(obj) if ((obj) == nullptr) return

#endif