#include "stdafx.h"

#include <cassert>

#include "game/game.h"
#include "game/world.h"
#include "game/event_manager.h"
#include "game/entity_controller.h"

#include "utility/keyboard.h"
#include "utility/view.h"

Game *gp_game = nullptr;

Game *CurrentGame()
{
	if (gp_game == nullptr) {
		gp_game = new Game;
	}
	return gp_game;
}

/*===================*\
|        Game         |
\*===================*/

Game::Game()
{
	p_world = nullptr;
	p_view_controller = nullptr;
	p_keyboard = nullptr;
}

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

void Game::setup()
{
	clear();
	
	p_world = make_shared<World>();
	p_world->setup();
	p_world->readFromFile("stage/last_save.txt");

	p_view_controller = make_shared<ViewController>();
	p_view_controller->setup(0.005f, -0.001f);

	p_keyboard = make_shared<Keyboard>();
	p_keyboard->setup();

	event_manager = make_shared<EventManager>();
	event_manager->setup();

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
}

void Game::clear()
{
}

void Game::tick(flt delta_time)
{
	// TODO
	getWorld()->tick(delta_time);

	getViewController()->tick(delta_time);
}
