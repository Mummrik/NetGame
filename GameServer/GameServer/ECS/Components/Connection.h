#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>

struct Connection
{
	uint32_t m_Id;
	asio::ip::udp::endpoint m_Endpoint;
	bool m_Authorized = false;
	uint64_t m_RoundtripTime = 255;
	float m_PingTimer;
};