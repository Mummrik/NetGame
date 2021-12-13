#include "Game.h"

Game::~Game()
{
	if (IsRunning())
	{
		Shutdown();
	}

	delete m_EntityManager;
	m_EntityManager = nullptr;
}

void Game::Start()
{
	std::cout << "\nInitialize ECS..." << std::endl;
	RegisterEntityComponents();
	RegisterEntitySystems();

	Entity world = m_EntityManager->CreateEntity();
	m_EntityManager->AddComponent(world, World{0, 1000,1000});

	m_IsRunning = true;
	m_Update = std::thread(&Game::Update, this);
	m_Network.Start();
}

void Game::Shutdown()
{
	m_IsRunning = false;
	m_Update.join();

	m_Network.Shutdown();
}

void Game::Update()
{
	std::clock_t current_time = clock();
	double accumulator = 0.0;

	while (IsRunning())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		std::clock_t new_time = clock();
		double frame_time = double(new_time - current_time) / CLOCKS_PER_SEC;
		current_time = new_time;

		accumulator += frame_time;

		while (IsRunning() && accumulator >= m_DeltaTime)
		{
			for (const std::shared_ptr<IEntitySystem>& system : m_EntityManager->GetSystems())
			{
				system->Update(this, m_DeltaTime);
			}

			accumulator -= m_DeltaTime;
			m_ElapsedTime++;
		}
	}
}

void Game::RegisterEntityComponents()
{
	std::clock_t current_time = clock();
	std::cout << "\tComponents...";

	// Add components here
	m_EntityManager->RegisterComponent<Connection>();
	m_EntityManager->RegisterComponent<Transform>();
	m_EntityManager->RegisterComponent<Movement>();
	m_EntityManager->RegisterComponent<World>();

	std::clock_t new_time = clock();
	float load_time = float(new_time - current_time) / CLOCKS_PER_SEC;
	std::cout << "\tDone '" << load_time << "s'" << std::endl;
}

void Game::RegisterEntitySystems()
{
	std::clock_t current_time = clock();
	std::cout << "\tSystems...";

	// Add Systems here
	m_EntityManager->RegisterSystem<ConnectionSystem>();
	m_EntityManager->RegisterSystem<MovementSystem>();
	m_EntityManager->RegisterSystem<WorldSystem>();

	SetSystemSignatures();
	std::clock_t new_time = clock();
	float load_time = float(new_time - current_time) / CLOCKS_PER_SEC;
	std::cout << "\tDone '" << load_time << "s'" << std::endl;
}

void Game::SetSystemSignatures()
{
	Signature signature;

	// Connection System
	signature.set(m_EntityManager->GetComponentType<Connection>());
	m_EntityManager->SetSystemSignature<ConnectionSystem>(signature);
	signature.reset();

	// Movement System
	signature.set(m_EntityManager->GetComponentType<Transform>());
	signature.set(m_EntityManager->GetComponentType<Movement>());
	m_EntityManager->SetSystemSignature<MovementSystem>(signature);
	signature.reset();

	// World System
	signature.set(m_EntityManager->GetComponentType<World>());
	m_EntityManager->SetSystemSignature<WorldSystem>(signature);
	signature.reset();
}
