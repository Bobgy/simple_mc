#include "stdafx.h"

#include <cassert>

#include "game/game.h"
#include "game/world.h"
#include "game/event_manager.h"
#include "game/entity_controller.h"

#include "utility/keyboard.h"
#include "utility/view.h"

#include "scripts/script.h"
#include "scripts/game/sg001.h"

Game *g_game = nullptr;

Game *CurrentGame()
{
	if (g_game == nullptr) {
		g_game = new Game;
	}
	return g_game;
}

/*===================*\
|        Game         |
\*===================*/

Game::~Game()
{
	clear();
}

World *Game::getWorld()
{
	assert(p_world);
	return p_world.get();
}

Player *Game::getPlayer()
{
	return getWorld()->getPlayer();
}

Entity *Game::getPlayerEntity()
{
	Player *player = getPlayer();
	if (player) return player->getEntity();
	return nullptr;
}

ViewController *Game::getViewController()
{
	assert(p_view_controller);
	return p_view_controller.get();
}

Keyboard *Game::getKeyboard()
{
	assert(p_keyboard);
	return p_keyboard.get();
}

EventManager *Game::getEventManager()
{
	return event_manager.get();
}

void Game::setup(shared_ptr<scripts::ScriptGame> script_game)
{
	if (!script_game) return;
	clear();

	m_script = script_game;

	p_world = make_shared<World>();
	p_view_controller = make_shared<ViewController>();
	p_keyboard = make_shared<Keyboard>();
	event_manager = make_shared<EventManager>();

	p_keyboard->setup();
	event_manager->setup();
	p_view_controller->setup(1.f, -1.f);
	p_world->setup(nullptr);

	m_script->setup_game();

}

void Game::clear()
{
	p_world.reset();
	p_view_controller.reset();
	p_keyboard.reset();
	event_manager.reset();
}

void Game::tick(flt delta_time)
{
	if (m_script) m_script->tick(delta_time);
	if (p_view_controller) p_view_controller->tick(delta_time);
	if (p_world) p_world->tick(delta_time);
}
