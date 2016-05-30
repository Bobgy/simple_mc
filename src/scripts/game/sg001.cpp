#include "stdafx.h"
#include "sg001.h"

#include "utility/event_board.h"
#include "utility/keyboard.h"
#include "utility/view.h"

#include "game/game.h"
#include "game/world.h"
#include "game/player.h"

#include "scripts/level/sl001.h"
#include "scripts/level/sl002.h"
#include "scripts/level/sl003.h"
#include "scripts/level/sl004.h"
#include "scripts/level/sl005.h"

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

vector<shared_ptr<function<void()>>> k_key_callback;

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

	auto level_script = make_shared<SL005>();
	level_script->setup();
#ifdef MAP_TUNNEL
	world->readFromFile("stage/tunnel.txt");
#else
	//world->readFromFile("stage/last_save.txt");
	world->randomGenerate(0, 100, 100.0f);
#endif
	world->m_game_play_range.m_min = Vec3i{-30, 1, -30};
	world->m_game_play_range.m_max = Vec3i{30, 1, 30};
	world->setup(level_script);
	

	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'w', STRING_ID("forward"));
	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'s', STRING_ID("backward"));
	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'p', STRING_ID("switch_display"));
	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'[', STRING_ID("simple_cube"));

	auto display_callback = make_shared<function<void(void)>>([](){
		bDisplay ^= 1;
	});
	if (display_callback) {
		k_key_callback.push_back(display_callback);
		event_manager->registerEventCallback(STRING_ID("switch_display"), k_key_callback.back(), EnumEventType::ON_UP);
	}

	auto simple_render_callback = make_shared<function<void(void)>>([]() {
		bCustomGLSL ^= 1;
		bSimpleCube ^= 1;
		CurrentGame()->getWorld()->changed = true;
	});
	if (simple_render_callback) {
		k_key_callback.push_back(simple_render_callback);
		event_manager->registerEventCallback(STRING_ID("simple_cube"), k_key_callback.back(), EnumEventType::ON_UP);
	}

	view_controller->setup(0.005f, -0.001f);
}
