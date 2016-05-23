#include <cassert>

#include "game/game.h"
#include "utility/vec.h"

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
	return p_world;
}

Player *Game::getPlayer()
{
	return getWorld()->getPlayer();
}

shared_ptr<Entity> Game::getPlayerEntity()
{
	return getPlayer()->getEntity();
}

ViewController *Game::getViewController()
{
	assert(p_view_controller);
	return p_view_controller;
}

Keyboard *Game::getKeyboard()
{
	assert(p_keyboard);
	return p_keyboard;
}

void Game::setup()
{
	clear();
	
	p_world = new World();
	p_world->setup();
	p_world->readFromFile("stage/last_save.txt");

	p_view_controller = new ViewController();
	p_view_controller->setup(0.005, -0.001f);

	p_keyboard = new Keyboard();
	p_keyboard->setup();
}

void Game::clear()
{
	if (p_world) delete p_world;
	if (p_view_controller) delete p_view_controller;
	if (p_keyboard) delete p_keyboard;

	assert(p_world == 0);
}

void Game::tick(flt delta_time)
{
	// TODO
	getWorld()->tick(delta_time);

	getViewController()->tick(delta_time);
}
