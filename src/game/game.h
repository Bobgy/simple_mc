#pragma once

#include "stdafx.h"

class World;
class Player;
class Entity;
class Keyboard;
class ViewController;
class Game;
class EventManager;

// get current game
Game *CurrentGame();

class Game {
protected:
	World *p_world;
	ViewController *p_view_controller;
	Keyboard *p_keyboard;
	shared_ptr<EventManager> event_manager;

public:
	Game();
	~Game();
	World *getWorld();
	Player *getPlayer();
	Entity *getPlayerEntity();
	ViewController *getViewController();
	Keyboard *getKeyboard();
	EventManager *getEventManager();

	void setup();
	void clear();
	void tick(flt delta_time);
};