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
	if (!g_game) {
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
	return getPlayer()->getEntity();
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
	p_world->setup();

	p_view_controller = make_shared<ViewController>();
	p_view_controller->setup(1.f, -1.f);

	p_keyboard = make_shared<Keyboard>();
	p_keyboard->setup();

	event_manager = make_shared<EventManager>();
	event_manager->setup();

	m_script->setup_game();

	Entity *player_entity = getPlayerEntity();
	auto mob = make_shared<Entity>(
		player_entity->get_pos() + Vec3f{3.0f, 0.0f, -4.0f},
		Vec3f::ZERO(),
		player_entity->getRadius(),
		player_entity->getHeight(),
		1.0f,
		true,
		true);
	shared_ptr<AIController> controller = make_shared<AIController>();
	mob->setup(controller);
	p_world->spawnEntity(mob);

	mob = make_shared<Entity>(
		player_entity->get_pos() + Vec3f{-3.0f, 0.0f, 4.0f},
		Vec3f::ZERO(),
		player_entity->getRadius(),
		player_entity->getHeight(),
		1.0f,
		true,
		true);
	controller = make_shared<AIController>();
	mob->setup(controller);
	p_world->spawnEntity(mob);
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
