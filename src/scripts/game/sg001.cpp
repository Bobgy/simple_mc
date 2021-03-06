#include "stdafx.h"
#include "sg001.h"

#include "utility/event_board.h"
#include "utility/keyboard.h"
#include "utility/view.h"

#include "game/game.h"
#include "game/world.h"
#include "game/player.h"

#include "scripts/script.h"
#include "scripts/level/levels.h"
#include "scripts/script_utility.h"

scripts::SG001::~SG001()
{
	// do nothing
}

void scripts::SG001::setup()
{
	// do nothing
}

void scripts::SG001::tick(flt delta_time)
{
	// do nothing
}

void scripts::SG001::setup_game()
{
	Game *game = CurrentGame();
	if (game == nullptr) return;

	Keyboard *keyboard = game->getKeyboard();
	EventManager *event_manager = game->getEventManager();
	World *world = game->getWorld();
	ViewController *view_controller = game->getViewController();
	if (view_controller == nullptr ||
		world == nullptr ||
		keyboard == nullptr ||
		event_manager == nullptr) {
		return;
	}

	auto level_script = MakeLevelScript();
	level_script->setup();
#ifdef MAP_TUNNEL
	world->readFromFile("stage/tunnel.txt");
	world->m_game_play_range.m_min = Vec3i{-30, 1, -30};
	world->m_game_play_range.m_max = Vec3i{30, 1, 30};
#else
	//world->readFromFile("stage/last_save.txt");
	world->randomGenerate(0, k_map_size, k_map_size, 100.0f);
	world->m_game_play_range.m_min = Vec3i{-k_map_size, 1, -k_map_size};
	world->m_game_play_range.m_max = Vec3i{k_map_size, 1, k_map_size};
#endif
	world->setup(level_script);

	InitKeys();

	view_controller->setup(0.005f, -0.001f);
}
