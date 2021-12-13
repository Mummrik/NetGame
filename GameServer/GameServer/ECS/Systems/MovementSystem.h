#pragma once
#include "IEntitySystem.h"

class MovementSystem : public IEntitySystem
{
public:
	void Update(Game* game, float dt) override;
};