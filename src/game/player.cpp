#include "player.h"
#include "game/game.h"

Player::Player()
{

}


Player::~Player()
{

}

Entity *Player::getEntity()
{
	if (m_possessed_entity_index < 0) {
		return nullptr;
	}
	return CurrentGame()->getWorld()->getEntity(m_possessed_entity_index);
}

void Player::setup()
{
	flt pp[3] = { 5.0f, 3.0f, 0.0f }, vv[3] = { 0.0f, 0.0f, 0.0f };
	Entity observer(pp, vv, r, h, 1.0);
	int index = CurrentGame()->getWorld()->spawnEntity(observer);
	possessEntity(index);
}
