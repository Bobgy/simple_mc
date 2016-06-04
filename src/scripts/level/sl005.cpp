#include "stdafx.h"

#include "sl005.h"

#include "game/game.h"
#include "game/world.h"
#include "game/entity.h"
#include "game/entity_controller.h"
#include "game/components/steering.h"

scripts::SL005::~SL005()
{
}

void scripts::SL005::setup()
{
}

typedef PriorityBasedAvoider EntityCtrl;
//typedef AIController EntityCtrl;

static flt len = 20.f;
static const int NUM = 23;
static bool bTurnAround = false;

void scripts::SL005::tick(flt delta_time)
{
	Game *game = CurrentGame();
	RETURN_IF_NULL(game);

	World *world = game->getWorld();
	RETURN_IF_NULL(world);
}

static size_t f[NUM];

void scripts::SL005::setup_level()
{
	Game *game = CurrentGame();
	if (game == nullptr) return;
	World *world = game->getWorld();
	if (world == nullptr) return;

	bCustomGLSL = false;
	bSimpleCube = true;
	CurrentGame()->getWorld()->changed = true;

	for (size_t i = 0; i < NUM; ++i) f[i] = i;
	random_shuffle(f, f + NUM);
	for (size_t num = 0; num < NUM; ++num) {
		flt ang = num / (flt)NUM * 2.0f * PI;
		auto mob = make_shared<Entity>(
			Vec3f{len * cos(ang), 1.0f, len * sin(ang)},
			Vec3f::ZERO(),
			0.45f,
			1.6f,
			1.0f,
			true,
			true);
		shared_ptr<AIController> controller = make_shared<EntityCtrl>();
		mob->setup(controller);
		mob->setPriority(Entity::Priority{(uint16_t)0, (uint16_t)f[num], (uint32_t)f[num]});
		//mob->setPriority(Entity::Priority{(uint16_t)0, (uint16_t)bLeft, (uint32_t)bLeft * 100});
		world->spawnEntity(mob);
		controller->setDestination(Vec3f{len * cos(ang + PI), 1.0f, len * sin(ang + PI)});
	}

	shared_ptr<Player> player = make_shared<Player>();
	world->addPlayer(player);

	player->getEntity()->m_rigid_body.m_position = Vec3f{0, 15, 15};
}
