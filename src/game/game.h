#pragma once

#include "stdafx.h"

//#include "scripts/script.h"
class World;
class Player;
class Entity;
class Keyboard;
class ViewController;
class Game;
class EventManager;

namespace scripts
{
	class ScriptGame;
};

// get current game
Game *CurrentGame();

class Game {
protected:
	shared_ptr<World> p_world;
	shared_ptr<ViewController> p_view_controller;
	shared_ptr<Keyboard> p_keyboard;
	shared_ptr<EventManager> event_manager;

	shared_ptr<scripts::ScriptGame> m_script;

public:
	Game() = default;
	~Game();
	World *getWorld();
	Player *getPlayer();
	Entity *getPlayerEntity();
	ViewController *getViewController();
	Keyboard *getKeyboard();
	EventManager *getEventManager();

	void setup(shared_ptr<scripts::ScriptGame> script_game);
	void clear();
	void tick(flt delta_time);
};