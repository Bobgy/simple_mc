#include "stdafx.h"

#include "player.h"
#include "game/game.h"
#include "game/world.h"
#include "game/entity.h"
#include "game/entity_controller.h"

Player::Player()
{

}


Player::~Player()
{

}

Entity *Player::getEntity()
{
	if (m_possessed_entity_index < 0) return nullptr;
	return CurrentGame()->getWorld()->getEntity(m_possessed_entity_index);
}

void Player::setup()
{
	Vec3f pp{ 0.0f, 4.0f, 5.0f }, vv{ 0.0f, 0.0f, 0.0f };
	shared_ptr<Entity> observer = make_shared<Entity>(pp, vv, 0.45f, 1.6f, 1.0f);
	auto pt = make_shared<PlayerController>();
	if (pt) {
		observer->setup(pt);
		observer->render_config.is_visible = false;
	}
	size_t index = CurrentGame()->getWorld()->spawnEntity(observer);
	possessEntity(index);
}
