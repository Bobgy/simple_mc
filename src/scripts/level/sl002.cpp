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

static flt len = 25.f;
static Vec3f pa = {len, 1, 0};
static Vec3f pb = {-len, 1, 0};
static int NUM = 400;
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

	bool finished = true;
	world->iterateEntityList([&finished](Entity *entity) {
		EntityController *controller = entity->getController();
		if (entity->isTicking() && controller && controller->isAI()) {
			AIController *ai = ASSERT_CAST<AIController*>(controller);
			bool success = ai->hasArrivedDestination(entity->m_rigid_body, ai->getDestination());
			if (success) {
				entity->disable();
			} else {
				finished = false;
			}
		}
	});

	if (finished) game->pause();
}

void scripts::SL002::setup_level()
{
	Game *game = CurrentGame();
	if (game == nullptr) return;
	World *world = game->getWorld();
	if (world == nullptr) return;

	//k_temporary_priority_last_ticks = 0;

	bCustomGLSL = false;
	bSimpleCube = true;
	CurrentGame()->getWorld()->changed = true;

	for (size_t num = 0; num < NUM; ++num) {
		//bool bLeft = num == 0;
		bool bLeft = num & 1;
		//bool bLeft = num < NUM / 2;
		auto mob = make_shared<Entity>(
			bLeft ? pa : pb,
			Vec3f::ZERO(),
			0.45f,
			1.6f,
			1.0f,
			true,
			true);
		shared_ptr<AIController> controller = make_shared<EntityCtrl>();
		mob->setup(controller, nullptr);
		mob->setPriority(Entity::Priority{(uint16_t)0, (uint16_t)num, (uint32_t)num});
		mob->m_rigid_body.m_yaw = bLeft ?  PI/2.0f : -PI / 2.0f;
		//mob->setPriority(Entity::Priority{(uint16_t)0, (uint16_t)bLeft, (uint32_t)bLeft * 100});
		world->spawnEntity(mob);
		controller->setDestination(bLeft ? pb : pa);
	}

	shared_ptr<Player> player = make_shared<Player>();
	world->addPlayer(player);

	player->getEntity()->m_rigid_body.m_position = Vec3f{0, 15, 15};
}
