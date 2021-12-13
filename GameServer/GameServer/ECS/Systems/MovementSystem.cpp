#include "MovementSystem.h"
#include "../GameServer/Game.h"

void MovementSystem::Update(Game* game, float dt)
{
	EntityManager* ecs = game->GetECS();

	for (const Entity& entity : m_Entities)
	{
		Transform& transform = ecs->GetComponent<Transform>(entity);
		Movement& movement = ecs->GetComponent<Movement>(entity);

		std::shared_ptr<WorldSystem> world_system = ecs->GetSystem<WorldSystem>();
		World& world = world_system->GetWorld(game, movement.m_WorldId);

		Vector3 new_position = transform.m_Position + movement.GetVelocity();

		if (world.IsInBounds(new_position * 0.01f) && new_position != transform.m_Position)
		{
			transform.m_Position = new_position;
			movement.m_Direction = { 0,0 };

			Vector2Int previous_tile_position = movement.m_TilePosition;
			movement.m_TilePosition = transform.m_Position.ToVector2Int() * 0.01f;

			if (movement.m_TilePosition != previous_tile_position)
			{
				//std::cout << movement.m_TilePosition << std::endl;
				// Send new map data
			}

			Connection& client = ecs->GetComponent<Connection>(entity);
			NetworkMessage msg(PacketType::Movement, client.m_Endpoint);
			msg.Write(client.m_Id);
			msg.Write(transform.m_Position);
			msg.Write(movement.m_Speed);
			game->GetNetwork()->SendToAll(msg);
		}
	}
}
