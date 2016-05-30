#include "stdafx.h"

#include "sl002.h"

#include "game/game.h"
#include "game/world.h"
#include "game/entity.h"
#include "game/entity_controller.h"
#include "game/components/steering.h"

scripts::SL002::~SL002()
{
}

void scripts::SL002::setup()
{
}

typedef PriorityBasedAvoider EntityCtrl;
//typedef AIController EntityCtrl;

static flt len = 20.f;
static Vec3f pa = {len, 1, 0};
static Vec3f pb = {-len, 1, 0};
static int NUM = 100;
static bool bTurnAround = false;

void scripts::SL002::tick(flt delta_time)
{
	Game *game = CurrentGame();
	RETURN_IF_NULL(game);

	World *world = game->getWorld();
	RETURN_IF_NULL(world);
	if (bTurnAround) {
		world->iterateEntityList([world](Entity *entity) {
			EntityController *controller = entity->getController();
			if (controller && controller->isAI()) {
				AIController *ai = ASSERT_CAST<EntityCtrl*>(controller);
				RigidBody &body = entity->m_rigid_body;
				if (AIController::hasArrivedDestination(body, ai->getDestination())) {
					ai->setDestination((pa == ai->getDestination()) ? pb : pa);
				}
			}
		});
	}
}

void scripts::SL002::setup_level()
{
	Game *game = CurrentGame();
	if (game == nullptr) return;
	World *world = game->getWorld();
	if (world == nullptr) return;
	for (size_t num = 0; num < NUM; ++num) {
		bool bLeft = num & 1;//num < NUM / 2;
		auto mob = make_shared<Entity>(
			bLeft ? pa : pb,
			Vec3f::ZERO(),
			0.45f,
			1.6f,
			1.0f,
			true,
			true);
		shared_ptr<AIController> controller = make_shared<EntityCtrl>();
		mob->setup(controller);
		mob->setPriority(Entity::Priority{(uint16_t)0, (uint16_t)num, num});
		world->spawnEntity(mob);
		controller->setDestination(bLeft ? pb : pa);
	}
}
