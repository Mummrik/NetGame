#pragma once
#include <thread>
#include <mutex>
#include "RpcManager.h"

class Game;
class Network
{
public:
	Network(Game* game, uint16_t port) : m_Game(game), m_Socket(m_Context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)), m_Rpc(this), m_NewPeerId(0), m_ReceiveBuffer(NET_MSG_MAX_SIZE) {}
	~Network();

	const uint16_t NET_MSG_MAX_SIZE = 65507;

	void Start();
	void Shutdown();
	void Send(NetworkMessage& msg);
	void SendToAll(NetworkMessage& msg, Connection* ignore = nullptr);
	void TerminateClient(const Connection& client);
	Game* GetGameInstance() { return m_Game; }
	void AckReceived(uint32_t sequence_id, const asio::ip::udp::endpoint& remote_endpoint);

private:
	Game* m_Game = nullptr;
	asio::io_context m_Context;
	asio::ip::udp::socket m_Socket;
	std::thread m_Listener;
	std::thread m_Dispatcher;
	std::vector<NetworkMessage> m_DispatchList;
	std::mutex m_DispatchMutex;
	RpcManager m_Rpc;
	void Listen();
	void Dispatch();
	void Handle(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& remote_endpoint);
	void Idle();
	uint32_t m_NewPeerId = 0;

	asio::ip::udp::endpoint m_RemoteEndpoint;
	std::vector<uint8_t> m_ReceiveBuffer;
	size_t m_ReceivedSize = 0;
};

