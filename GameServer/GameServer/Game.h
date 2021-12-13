#pragma once
#include <thread>
#include <iostream>
#include "Network.h"
#include "ECS/EntityManager.h"

// Entity Components
#include "ECS/Components/Connection.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/World.h"

// Entity Systems
#include "ECS/Systems/ConnectionSystem.h"
#include "ECS/Systems/MovementSystem.h"
#include "ECS/Systems/WorldSystem.h"

class Game
{
public:
	Game(uint16_t port) : m_Network(this, port), m_EntityManager(new EntityManager) {}
	~Game();

	bool IsRunning() { return m_IsRunning; }

	void Start();
	void Shutdown();
	void Update();
	EntityManager* GetECS() { return m_EntityManager; }
	Network* GetNetwork() { return &m_Network; }
	uint64_t GetElapsedTime() { return m_ElapsedTime; }

private:
	void RegisterEntityComponents();
	void RegisterEntitySystems();
	void SetSystemSignatures();

private:
	bool m_IsRunning = false;
	uint64_t m_ElapsedTime = 0;
	const float m_DeltaTime = 0.01f;
	std::thread m_Update;
	Network m_Network;
	EntityManager* m_EntityManager = nullptr;
};

