#include "stdafx.h"
#include "sg001.h"

#include "utility/event_board.h"
#include "utility/keyboard.h"
#include "utility/view.h"

#include "game/game.h"
#include "game/world.h"
#include "game/player.h"

#include "scripts/level/sl001.h"

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

	auto level_script = make_shared<SL001>();
	level_script->setup();
	world->setup(level_script);

	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'w', STRING_ID("forward"));
	event_manager->registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'s', STRING_ID("backward"));

	view_controller->setup(0.005f, -0.001f);

	world->randomGenerate(0, 30);
	//world->readFromFile("stage/last_save.txt");

	shared_ptr<Player> player = make_shared<Player>();
	world->addPlayer(player);
}
