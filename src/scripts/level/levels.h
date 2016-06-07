#pragma once

#include <memory>

#include "scripts/script.h"

#include "scripts/level/sl001.h"
#include "scripts/level/sl002.h"
#include "scripts/level/sl003.h"
#include "scripts/level/sl004.h"
#include "scripts/level/sl005.h"

inline shared_ptr<scripts::ScriptLevel> MakeLevelScript()
{
	shared_ptr<scripts::ScriptLevel> ret;
	if (g_selected_level_script == 1) {
		ret = make_shared<scripts::SL001>();
	} else if (g_selected_level_script == 2) {
		ret = make_shared<scripts::SL002>();
	} else if (g_selected_level_script == 3) {
		ret = make_shared<scripts::SL003>();
	} else if (g_selected_level_script == 4) {
		ret = make_shared<scripts::SL004>();
	} else if (g_selected_level_script == 5) {
		ret = make_shared<scripts::SL005>();
	}
	if (ret) return ret;
	return nullptr;
}