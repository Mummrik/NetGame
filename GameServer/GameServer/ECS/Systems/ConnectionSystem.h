#pragma once
#include "IEntitySystem.h"

class ConnectionSystem : public IEntitySystem
{
public:
	void Update(Game* game, float dt) override;

private:
	void Ping(Game* game, float dt);
	uint64_t m_PingInterval = 3;
};

