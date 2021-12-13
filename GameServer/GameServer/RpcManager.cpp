#include "RpcManager.h"
#include "Network.h"
#include "Game.h"

void RpcManager::Invoke(const PacketType& type, Connection& client, NetworkMessage& data)
{
	if (type < PacketType::MAX_SIZE && m_Rpc[(uint16_t)type] != nullptr)
	{
		std::invoke(m_Rpc[(uint16_t)type], this, client, data);
	}
	else
	{
		std::cout << "[RPC] Received unknown packet type '" << (uint16_t)type << "' from " << client.m_Endpoint << std::endl;
	}
}

void RpcManager::Init()
{
	m_Rpc[(uint16_t)PacketType::Disconnect]		= &RpcManager::Disconnect;
	m_Rpc[(uint16_t)PacketType::HandShake]		= &RpcManager::HandShake;
	m_Rpc[(uint16_t)PacketType::Acknowledge]	= &RpcManager::Acknowledge;
	m_Rpc[(uint16_t)PacketType::Ping]			= &RpcManager::Ping;
	m_Rpc[(uint16_t)PacketType::Movement]		= &RpcManager::MovementInput;
}

void RpcManager::Disconnect(Connection& client, NetworkMessage& data)
{
	client.m_Authorized = false;

	NetworkMessage msg(PacketType::RemoveCreature, client.m_Endpoint, true);
	msg.Write(client.m_Id);
	//m_Network->SendToAll(msg, &client);

	m_Network->TerminateClient(client);
}

void RpcManager::HandShake(Connection& client, NetworkMessage& data)
{
	if (client.m_Authorized)
	{
		return;
	}

	if (Game* game = m_Network->GetGameInstance())
	{
		client.m_Authorized = true;
		uint64_t time = data.ReadUint64();

		client.m_RoundtripTime = m_Network->GetGameInstance()->GetElapsedTime() - time;

		EntityManager* ecs = game->GetECS();
		Entity entity = ecs->GetEntityFromEndpoint(client.m_Endpoint);

		Transform& transform = ecs->AddComponent(entity, Transform{});
		Movement& movement = ecs->AddComponent(entity, Movement{});
		movement.m_Speed = 5.f;
		movement.m_TilePosition = transform.m_Position.ToVector2Int() * 0.01f;

		std::vector<Entity> entities = ecs->GetEntities();
		for (Entity entity : entities)
		{
			const Connection& other_client = ecs->GetComponent<Connection>(entity);
			if (other_client.m_Id == client.m_Id)
			{
				continue;
			}

			const Transform& transform = ecs->GetComponent<Transform>(entity);
			NetworkMessage msg(PacketType::PlayerData, client.m_Endpoint, true);
			msg.Write(other_client.m_Id);
			msg.Write(transform.m_Position);
			//m_Network->Send(msg);
		}

		NetworkMessage msg(PacketType::PlayerData, client.m_Endpoint, true);
		msg.Write(client.m_Id);
		msg.Write(transform.m_Position);
		//m_Network->SendToAll(msg, nullptr);

	}
}

void RpcManager::Acknowledge(Connection& client, NetworkMessage& data)
{
	m_Network->AckReceived(data.GetSequenceId(), client.m_Endpoint);
}

void RpcManager::Ping(Connection& client, NetworkMessage& data)
{
	uint64_t time = data.ReadUint64();
	uint64_t elapsed_time = m_Network->GetGameInstance()->GetElapsedTime();

	client.m_RoundtripTime = elapsed_time - time;
}

void RpcManager::MovementInput(Connection& client, NetworkMessage& data)
{
	if (Game* game = m_Network->GetGameInstance())
	{
		EntityManager* ecs = game->GetECS();
		Vector2 direction = data.ReadVector2();
		
		Entity entity = ecs->GetEntityFromEndpoint(client.m_Endpoint);
		Movement& movement = ecs->GetComponent<Movement>(entity);
		movement.m_Direction = direction.Normalize();
	}
}
