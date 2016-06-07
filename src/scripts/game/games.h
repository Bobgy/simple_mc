#pragma once

#include <memory>

#include "scripts/script.h"
#include "scripts/game/sg001.h"
#include "scripts/game/sg002.h"

inline shared_ptr<scripts::ScriptGame> MakeGameScript()
{
	shared_ptr<scripts::ScriptGame> ret;
	if (g_selected_game_script == 1) {
		ret = make_shared<scripts::SG001>();
	} else if (g_selected_game_script == 2) {
		ret = make_shared<scripts::SG002>();
	}
	if (ret) return ret;
	return nullptr;
}