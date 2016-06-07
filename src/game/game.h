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
	uint32_t m_tick_count = 0;
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
	uint32_t getTickCount() { return m_tick_count; }
	void pause() { GAME_SPEED = 0.f; }
	void resume() { GAME_SPEED = 1.f; }
	void slowMotion() { GAME_SPEED = 0.5f; }

	void setup(shared_ptr<scripts::ScriptGame> script_game);
	void clear();
	void tick(flt delta_time);
};