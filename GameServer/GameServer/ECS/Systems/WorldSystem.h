#pragma once
#include "IEntitySystem.h"

struct World;
class WorldSystem : public IEntitySystem
{
public:
	void Update(Game* game, float dt) override;
	World& GetWorld(Game* game, uint16_t world_id);
};

