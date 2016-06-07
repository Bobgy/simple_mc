#include "stdafx.h"

#include "sl003.h"

#include "game/game.h"
#include "game/world.h"
#include "game/entity.h"
#include "game/entity_controller.h"
#include "game/components/steering.h"

scripts::SL003::~SL003()
{
}

void scripts::SL003::setup()
{
}

static flt len = 8.f, len2 = 6.f;
static Vec3f p[4] = {
	{  len, 1, -0.5},
	{ len2, 1, -0.5},
	{-len2, 1, -0.5},
	{ -len, 1, -0.5}
};
static int pp[100];
static int dir[100];

void scripts::SL003::tick(flt delta_time)
{
	Game *game = CurrentGame();
	RETURN_IF_NULL(game);

	World *world = game->getWorld();
	RETURN_IF_NULL(world);
	world->iterateEntityList([world](Entity *entity) {
		EntityController *controller = entity->getController();
		if (controller && controller->isAI()) {
			AIController *ai = ASSERT_CAST<PriorityBasedAvoider*>(controller);
			RigidBody &body = entity->m_rigid_body;
			if (AIController::hasArrivedDestination(body, ai->getDestination())) {
				size_t id = entity->getID();
				if (pp[id] > 0 && pp[id] < 3) {
					ai->setDestination(p[pp[id] += dir[id]]);
				}
			}
		}
	});
}

void scripts::SL003::setup_level()
{
	Game *game = CurrentGame();
	if (game == nullptr) return;
	World *world = game->getWorld();
	if (world == nullptr) return;
	for (size_t num = 0; num < NUM; ++num) {
		bool bSide = num < NUM / 2;
		dir[num] = bSide ? 1 : -1;
		Vec3f pos;
		auto mob = make_shared<Entity>(
			bSide ? p[0] : p[3],
			Vec3f::ZERO(),
			0.45f,
			1.6f,
			1.0f,
			true,
			true);
		shared_ptr<AIController> controller = make_shared<PriorityBasedAvoider>();
		mob->setup(controller, nullptr);
		mob->setPriority(Entity::Priority{(uint16_t)0, (uint16_t)num, num});
		world->spawnEntity(mob);
		controller->setDestination(bSide ? p[1] : p[2]);
		pp[num] = bSide ? 1 : 2;
	}

	shared_ptr<Player> player = make_shared<Player>();
	world->addPlayer(player);

	player->getEntity()->m_rigid_body.m_position = Vec3f{10, 10, 10};
}
