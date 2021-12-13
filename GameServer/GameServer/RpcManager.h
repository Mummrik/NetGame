#pragma once
#include "NetworkMessage.h"

class Network;
struct Connection;
class RpcManager
{
public:
	RpcManager(Network* network) : m_Network(network) { Init(); }

	void Invoke(const PacketType& type, Connection& client, NetworkMessage& data);

private:
	Network* m_Network;
	typedef void(RpcManager::* RpcCallbacks)(Connection&, NetworkMessage&);
	RpcCallbacks m_Rpc[(size_t)PacketType::MAX_SIZE];

private:
	void Init();
	void Disconnect(Connection& client, NetworkMessage& data);
	void HandShake(Connection& client, NetworkMessage& data);
	void Acknowledge(Connection& client, NetworkMessage& data);
	void Ping(Connection& client, NetworkMessage& data);
	void MovementInput(Connection& client, NetworkMessage& data);
};

