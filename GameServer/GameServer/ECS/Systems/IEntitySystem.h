#pragma once
#include <set>

using Entity = uint32_t;

class Game;
class IEntitySystem
{
public:
	std::set<Entity> m_Entities;
	virtual void Update(Game* game, float dt) = 0;
};