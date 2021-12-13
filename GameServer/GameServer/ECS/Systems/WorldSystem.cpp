#include "WorldSystem.h"
#include "../GameServer/Game.h"

void WorldSystem::Update(Game* game, float dt)
{
}

World& WorldSystem::GetWorld(Game* game, uint16_t world_id)
{
	EntityManager* ecs = game->GetECS();
	for (const Entity& entity : m_Entities)
	{
		World& world = ecs->GetComponent<World>(entity);

		if (world.m_Id == world_id)
		{
			return world;
		}
	}

	World world;
	return world;
}
