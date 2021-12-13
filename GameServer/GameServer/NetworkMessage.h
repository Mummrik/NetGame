#pragma once
#include <iostream>
#include <vector>
#include "VectorMath.h"
#include "ECS/Components/Connection.h"

enum class PacketType : uint16_t
{
	Disconnect,
	HandShake,
	Acknowledge,
	Ping,
	Notify,
	Message,
	PlayerData,
	Movement,
	RemoveCreature,
	Rotation,
	MapSector,
	MapData,
	UnloadMapData,
	MAX_SIZE	// This need to be last
};

static uint32_t s_SequenceCounter;
const uint8_t MAX_MSG_TIMEOUTS = 32;	// Max amount of nack's before msg is timedout

class NetworkMessage
{
private:
	asio::ip::udp::endpoint m_Endpoint;
	std::vector<uint8_t> m_Data{};
	PacketType m_Type;
	uint32_t m_SequenceId = 0;
	size_t m_Size = 0;
	size_t m_Index = 0;
	uint64_t m_DispatchTimestamp = 0;
	uint8_t m_SendTimeout = 0;

public:

	// Construct NetworkMessage of a given type
	NetworkMessage(const PacketType& type, const asio::ip::udp::endpoint& receiver_endpoint, bool reliable = false) : m_Type(type), m_Endpoint(receiver_endpoint)
	{
		if (s_SequenceCounter == UINT32_MAX)
		{
			s_SequenceCounter = 0;
			std::cout << "Reached max sequence id size, reseting" << std::endl;
		}

		if (reliable)
		{
			m_SequenceId = ++s_SequenceCounter;
		}
		Write(m_SequenceId);

		// Add header to body
		Write((uint16_t)type);
	}

	// Construct NetworkMessage from received data
	NetworkMessage(const std::vector<uint8_t>& data) : m_Size(data.size())
	{
		// Copy data to this network message
		m_Data.reserve(m_Size);
		for (size_t i = 0; i < m_Size; ++i)
		{
			m_Data.push_back(data[i]);
		}

		m_SequenceId = ReadUint32();
		m_Type = (PacketType)ReadUint16();
	}

	// Get data size
	size_t GetSize()
	{
		return m_Size;
	}

	uint32_t GetSequenceId()
	{
		return m_SequenceId;
	}

	// Get packet type identifier
	PacketType GetType()
	{
		return m_Type;
	}

	// Get array of byte data
	//std::array<uint8_t, NET_MSG_BUFFER_SIZE> GetData()
	std::vector<uint8_t> GetData()
	{
		return m_Data;
	}

	void SetEndpoint(const asio::ip::udp::endpoint& endpoint)
	{
		m_Endpoint = endpoint;
	}

	const asio::ip::udp::endpoint& GetEndpoint()
	{
		return m_Endpoint;
	}

	uint64_t GetDispatchTimestamp()
	{
		return m_DispatchTimestamp;
	}

	void SetDispatchTimestamp(uint64_t timestamp)
	{
		m_DispatchTimestamp = timestamp;
	}

	bool Timeout()
	{
		return ++m_SendTimeout >= MAX_MSG_TIMEOUTS;
	}

	void Write(const std::vector<uint8_t>& values)
	{
		for (uint8_t byte : values)
		{
			Write(byte);
		}
	}

	void Write(uint8_t value)
	{
		//m_PacketData[m_Index++] = value;
		//m_Size++;
		m_Data.push_back(value);
		m_Size = m_Data.size();
	}

	void Write(int8_t value)
	{
		Write((uint8_t)value);
	}


	void Write(int16_t value)
	{
		for (size_t i = 0; i < sizeof(int16_t); ++i)
		{
			Write((uint8_t)(value >> 8 * i));
		}
	}

	void Write(uint16_t value)
	{
		for (size_t i = 0; i < sizeof(uint16_t); ++i)
		{
			Write((uint8_t)(value >> 8 * i));
		}
	}

	void Write(int32_t value)
	{
		for (size_t i = 0; i < sizeof(int32_t); ++i)
		{
			Write((uint8_t)(value >> 8 * i));
		}
	}

	void Write(uint32_t value)
	{
		for (size_t i = 0; i < sizeof(uint32_t); ++i)
		{
			Write((uint8_t)(value >> 8 * i));
		}
	}

	void Write(int64_t value)
	{
		for (size_t i = 0; i < sizeof(int64_t); ++i)
		{
			Write((uint8_t)(value >> 8 * i));
		}
	}

	void Write(uint64_t value)
	{
		for (size_t i = 0; i < sizeof(uint64_t); ++i)
		{
			Write((uint8_t)(value >> 8 * i));
		}
	}

	void Write(float value)
	{
		uint8_t result[sizeof(float)];
		memcpy(result, &value, sizeof(float));

		for (size_t i = 0; i < sizeof(float); ++i)
		{
			Write(result[i]);
		}
	}

	void Write(const Vector3& value)
	{
		Write((float)value.x);
		Write((float)value.y);
		Write((float)value.z);
	}

	void Write(const Vector2& value)
	{
		Write((float)value.x);
		Write((float)value.y);
	}

	uint8_t ReadUint8()
	{
		return m_Data[m_Index++];
	}

	int8_t ReadInt8()
	{
		return (int8_t)ReadUint8();
	}

	uint16_t ReadUint16()
	{
		uint16_t value = 0;
		for (size_t i = 0; i < sizeof(uint16_t); ++i)
		{
			value += (uint16_t)ReadUint8() << (8 * i);
		}

		return value;
	}

	int16_t ReadInt16()
	{
		int16_t value = 0;
		for (size_t i = 0; i < sizeof(int16_t); ++i)
		{
			value += (int16_t)ReadUint8() << (8 * i);
		}

		return value;
	}

	uint32_t ReadUint32()
	{
		uint32_t value = 0;
		for (size_t i = 0; i < sizeof(uint32_t); ++i)
		{
			value += (uint32_t)ReadUint8() << (8 * i);
		}

		return value;
	}

	int32_t ReadInt32()
	{
		int32_t value = 0;
		for (size_t i = 0; i < sizeof(int32_t); ++i)
		{
			value += (int32_t)ReadUint8() << (8 * i);
		}

		return value;
	}

	uint64_t ReadUint64()
	{
		uint64_t value = 0;
		for (size_t i = 0; i < sizeof(uint64_t); ++i)
		{
			value += (uint64_t)ReadUint8() << (8 * i);
		}

		return value;
	}

	int64_t ReadInt64()
	{
		int64_t value = 0;
		for (size_t i = 0; i < sizeof(int64_t); ++i)
		{
			value += (int64_t)ReadUint8() << (8 * i);
		}

		return value;
	}

	float ReadFloat()
	{
		uint8_t buffer[sizeof(float)];
		for (size_t i = 0; i < sizeof(float); ++i)
		{
			buffer[i] = ReadUint8();
		}
		return (*(float*)buffer);
	}

	const Vector3& ReadVector3()
	{
		//TODO: Clean up this function!!!
		Vector3 ret;
		uint8_t buffer[sizeof(float)];
		for (size_t i = 0; i < sizeof(float); ++i)
		{
			buffer[i] = ReadUint8();
		}
		ret.x = (*(float*)buffer);

		for (size_t i = 0; i < sizeof(float); ++i)
		{
			buffer[i] = ReadUint8();
		}
		ret.y = (*(float*)buffer);

		for (size_t i = 0; i < sizeof(float); ++i)
		{
			buffer[i] = ReadUint8();
		}
		ret.z = (*(float*)buffer);

		return ret;
	}

	const Vector2& ReadVector2()
	{
		//TODO: Clean up this function!!!
		Vector2 ret;
		uint8_t buffer[sizeof(float)];
		for (size_t i = 0; i < sizeof(float); ++i)
		{
			buffer[i] = ReadUint8();
		}
		ret.x = (*(float*)buffer);

		for (size_t i = 0; i < sizeof(float); ++i)
		{
			buffer[i] = ReadUint8();
		}
		ret.y = (*(float*)buffer);

		return ret;
	}
};