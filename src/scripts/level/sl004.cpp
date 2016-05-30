#include "stdafx.h"

#include "sl004.h"

#include "game/game.h"
#include "game/world.h"
#include "game/entity.h"
#include "game/entity_controller.h"
#include "game/components/steering.h"

scripts::SL004::~SL004()
{
}

void scripts::SL004::setup()
{
}

static flt len = 8.f, len2 = 6.f;
static Vec3f p[4] = {
	{  len, 1, -0.5},
	{ len2, 1, -0.5},
	{-len2, 1, -0.5},
	{ -len, 1, -0.5}
};
const static int NUM = 20;

void scripts::SL004::tick(flt delta_time)
{
	Game *game = CurrentGame();
	RETURN_IF_NULL(game);

	if (game->getTickCount() == 120) {
		EntityController* controller = game->getWorld()->getEntity(0)->getController();
		ASSERT_CAST<PriorityBasedAvoider*>(controller)->setDestination(p[4]);
	}
}

void scripts::SL004::setup_level()
{
	Game *game = CurrentGame();
	if (game == nullptr) return;
	World *world = game->getWorld();
	if (world == nullptr) return;
	for (size_t num = 0; num < NUM; ++num) {
		auto mob = make_shared<Entity>(
			p[0],
			Vec3f::ZERO(),
			0.45f,
			1.6f,
			1.0f,
			true,
			true);
		shared_ptr<AIController> controller = make_shared<PriorityBasedAvoider>();
		mob->setup(controller);
		mob->setPriority(Entity::Priority{(uint16_t)0, (uint16_t)num, num});
		world->spawnEntity(mob);
		controller->setDestination(p[0]);
	}
}
