#include "ConnectionSystem.h"
#include "../GameServer/Game.h"

void ConnectionSystem::Update(Game* game, float dt)
{
	Ping(game, dt);
}

void ConnectionSystem::Ping(Game* game, float dt)
{
	EntityManager* ecs = game->GetECS();

	for (const Entity& entity : m_Entities)
	{
		Connection& client = ecs->GetComponent<Connection>(entity);

		if (client.m_Authorized)
		{
			client.m_PingTimer -= dt;
			if (client.m_PingTimer <= 0)
			{
				NetworkMessage msg(PacketType::Ping, client.m_Endpoint);
				msg.Write(client.m_RoundtripTime);
				msg.Write(game->GetElapsedTime());
				game->GetNetwork()->Send(msg);

				client.m_PingTimer = m_PingInterval;
			}
		}
	}
}
