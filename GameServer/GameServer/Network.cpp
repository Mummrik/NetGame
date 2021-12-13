#include "Network.h"
#include "Game.h"
#include "NetworkMessage.h"

Network::~Network()
{
	if (m_Game->IsRunning())
	{
		m_Game->Shutdown();
	}
}

void Network::Start()
{
	std::cout << "\nInitialize Network..." << std::endl;
	m_Listener = std::thread(&Network::Listen, this);
}

void Network::Shutdown()
{
	m_Socket.close();

	m_Listener.join();
	m_Dispatcher.join();
}

void Network::Send(NetworkMessage& msg)
{
	if (msg.GetSequenceId() > 0)
	{
		//std::cout << "Send type: " << (int)msg.GetType() << " Sequence: " << msg.GetSequenceId() << std::endl;
		msg.SetDispatchTimestamp(m_Game->GetElapsedTime());
		m_Socket.send_to(asio::buffer(msg.GetData()), msg.GetEndpoint());

		m_DispatchMutex.lock();
		m_DispatchList.push_back(msg);
		m_DispatchMutex.unlock();
	}
	else
	{
		m_Socket.send_to(asio::buffer(msg.GetData()), msg.GetEndpoint());
	}
}

void Network::SendToAll(NetworkMessage& msg, Connection* ignore)
{
	EntityManager* ecs = m_Game->GetECS();
	std::vector<Entity> entities = ecs->GetEntities();

	for (Entity entity : entities)
	{
		const Connection& client = ecs->GetComponent<Connection>(entity);
		if (client.m_Endpoint.size() > 0)
		{
			if (ignore)
			{
				if (client.m_Endpoint != ignore->m_Endpoint)
				{
					if (msg.GetSequenceId() > 0)
					{
						// Add to reliable queue
						NetworkMessage copy_msg(msg.GetData());
						copy_msg.SetEndpoint(client.m_Endpoint);
						copy_msg.SetDispatchTimestamp(m_Game->GetElapsedTime());
						m_Socket.send_to(asio::buffer(copy_msg.GetData()), copy_msg.GetEndpoint());

						m_DispatchMutex.lock();
						m_DispatchList.push_back(copy_msg);
						m_DispatchMutex.unlock();
					}
					else
					{
						msg.SetEndpoint(client.m_Endpoint);
						m_Socket.send_to(asio::buffer(msg.GetData()), msg.GetEndpoint());
					}
				}
			}
			else
			{
				if (msg.GetSequenceId() > 0)
				{
					// Add to reliable queue
					NetworkMessage copy_msg(msg.GetData());
					copy_msg.SetEndpoint(client.m_Endpoint);
					copy_msg.SetDispatchTimestamp(m_Game->GetElapsedTime());
					m_Socket.send_to(asio::buffer(copy_msg.GetData()), copy_msg.GetEndpoint());

					m_DispatchMutex.lock();
					m_DispatchList.push_back(copy_msg);
					m_DispatchMutex.unlock();
				}
				else
				{
					msg.SetEndpoint(client.m_Endpoint);
					m_Socket.send_to(asio::buffer(msg.GetData()), msg.GetEndpoint());
				}
			}
		}
	}
}

void Network::TerminateClient(const Connection& client)
{
	std::cout << "New Disconnect | " << client.m_Endpoint << " | id: " << client.m_Id << std::endl;
	NetworkMessage msg(PacketType::Disconnect, client.m_Endpoint, true);
	Send(msg);

	EntityManager* ecs = m_Game->GetECS();
	Entity entity = ecs->GetEntityFromEndpoint(client.m_Endpoint);
	if (entity <= MAX_ENTITIES)
	{
		ecs->DestroyEntity(entity);
	}
}

void Network::Listen()
{
	std::cout << "\tListner thread...\tRunning" << std::endl;
	m_Dispatcher = std::thread(&Network::Dispatch, this);

	while (m_Game->IsRunning())
	{
		try
		{
			m_ReceivedSize = m_Socket.receive_from(asio::buffer(m_ReceiveBuffer), m_RemoteEndpoint);
		}
		catch (const std::exception& ex)
		{
			if (m_Game->IsRunning())
			{
				std::cout << "\nException: " << ex.what() << "\n" << std::endl;
			}
		}

		if (m_Game->IsRunning() && m_ReceivedSize > 0)
		{
			Handle(m_ReceiveBuffer, m_RemoteEndpoint);
		}
	}
}

void Network::Dispatch()
{
	std::unordered_map<uint32_t, const asio::ip::udp::endpoint&> timeout_msgs;
	std::cout << "\tDispatch thread...\tRunning\n" << std::endl;
	while (m_Game->IsRunning())
	{
		if (m_DispatchList.size() > 0)
		{
			m_DispatchMutex.lock();
			for (NetworkMessage& msg : m_DispatchList)
			{
				EntityManager* ecs = m_Game->GetECS();
				Entity entity = ecs->GetEntityFromEndpoint(msg.GetEndpoint());
				Connection& connection = ecs->GetComponent<Connection>(entity);

				uint64_t timestamp = m_Game->GetElapsedTime() - msg.GetDispatchTimestamp();
				if (timestamp >= connection.m_RoundtripTime)
				{
					msg.SetDispatchTimestamp(m_Game->GetElapsedTime());
					m_Socket.send_to(asio::buffer(msg.GetData()), msg.GetEndpoint());

					if (msg.Timeout())
					{
						timeout_msgs.emplace(msg.GetSequenceId(), msg.GetEndpoint());
					}
				}

			}
			m_DispatchMutex.unlock();

			for (const auto& msg : timeout_msgs)
			{
				AckReceived(msg.first, msg.second);
			}
			timeout_msgs.clear();
		}
		else
		{
			Idle();
		}
	}
}

void Network::Handle(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& remote_endpoint)
{
	if (EntityManager* ecs = m_Game->GetECS())
	{
		Entity entity = ecs->GetEntityFromEndpoint(remote_endpoint);
		if (entity <= MAX_ENTITIES)
		{
			NetworkMessage msg(data);
			msg.SetEndpoint(remote_endpoint);
			Connection& client = ecs->GetComponent<Connection>(entity);
			if (client.m_Authorized)
			{
				m_Rpc.Invoke(msg.GetType(), client, msg);
			}
			else
			{
				//std::cout << "Recv type: " << (int)msg.GetType() << " Sequence: " << msg.GetSequenceId() << std::endl;
				if (msg.GetType() == PacketType::HandShake || msg.GetType() == PacketType::Acknowledge)
				{
					m_Rpc.Invoke(msg.GetType(), client, msg);
				}
				else
				{
					TerminateClient(client);
				}
			}
		}
		else
		{
			// No entity with this endpoint exists lets create one
			uint32_t peer_id = ++m_NewPeerId;	// TODO: Proper UUID generation
			Connection& client = ecs->AddComponent(ecs->CreateEntity(), Connection{ peer_id, remote_endpoint });

			NetworkMessage msg(PacketType::HandShake, client.m_Endpoint, true);
			msg.Write(client.m_Id);
			msg.Write(m_Game->GetElapsedTime());
			Send(msg);

			std::cout << "New Connection | " << client.m_Endpoint << " | id: " << client.m_Id << std::endl;
		}
	}
}

void Network::AckReceived(uint32_t sequence_id, const asio::ip::udp::endpoint& remote_endpoint)
{
	m_DispatchMutex.lock();
	std::vector<NetworkMessage>::iterator itr = std::find_if(m_DispatchList.begin(), m_DispatchList.end(), 
		[sequence_id, remote_endpoint](NetworkMessage& msg)
		{ 
			return msg.GetSequenceId() == sequence_id && msg.GetEndpoint() == remote_endpoint;
		});

	if (itr != m_DispatchList.end())
	{
		m_DispatchList.erase(itr);
	}
	m_DispatchMutex.unlock();
}

void Network::Idle()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
