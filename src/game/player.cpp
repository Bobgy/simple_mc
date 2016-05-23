#include "player.h"
#include "game/game.h"
#include "game/entity_controller.h"

Player::Player()
{

}


Player::~Player()
{

}

shared_ptr<Entity> Player::getEntity()
{
	if (m_possessed_entity_index < 0) {
		assert(0);
		return nullptr;
	}
	return CurrentGame()->getWorld()->getEntity(m_possessed_entity_index);
}

void Player::setup()
{
	flt pp[3] = { 5.0f, 3.0f, 0.0f }, vv[3] = { 0.0f, 0.0f, 0.0f };
	shared_ptr<Entity> observer(new Entity(pp, vv, r, h, 1.0));
	auto pt = make_shared<PlayerController>();
	if (pt) {
		observer->setup(pt);
	}
	int index = CurrentGame()->getWorld()->spawnEntity(observer);
	possessEntity(index);
}
