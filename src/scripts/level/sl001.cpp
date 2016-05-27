#include "stdafx.h"

#include "sl001.h"

#include "game/game.h"
#include "game/world.h"

scripts::SL001::~SL001()
{
}

void scripts::SL001::setup()
{
}

void scripts::SL001::tick(flt delta_time)
{
}

void scripts::SL001::setup_level()
{
	Game *game = CurrentGame();
	if (game == nullptr) return;
	World *world = game->getWorld();
	if (world == nullptr) return;

	for (size_t num = 0; num < 4; ++num) {
		auto mob = make_shared<Entity>(
			Vec3f{3.0f, 1.0f, -4.0f},
			Vec3f::ZERO(),
			0.45f,
			1.6f,
			1.0f,
			true,
			true);
		shared_ptr<AIController> controller = make_shared<AIController>();
		mob->setup(controller);
		world->spawnEntity(mob);
	}
}
