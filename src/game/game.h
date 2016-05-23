#pragma once

#include <memory>

#include "game/world.h"
#include "utility/vec.h"
#include "utility/view.h"
#include "utility/keyboard.h"

class Game;
// get current game
Game *CurrentGame();

class Game {
protected:
	World *p_world;
	ViewController *p_view_controller;
	Keyboard *p_keyboard;

public:
	Game();
	~Game();
	World *getWorld();
	Player *getPlayer();
	shared_ptr<Entity> getPlayerEntity();
	ViewController *getViewController();
	Keyboard *getKeyboard();

	void setup();
	void clear();
	void tick(flt delta_time);
};