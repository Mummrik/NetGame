#pragma once
#include <queue>
#include <bitset>
#include <cassert>
#include <array>
#include <unordered_map>
#include <memory>
#include "Systems/IEntitySystem.h"

using Entity = uint32_t;
using ComponentType = uint16_t;

const Entity MAX_ENTITIES = 1000000;		// Total amount of entities the system can hold
const ComponentType MAX_COMPONENTS = 32;	// How many components each entity can hold
using Signature = std::bitset<MAX_COMPONENTS>;

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray
{
public:
	void InsertData(Entity entity, T component)
	{
		assert(m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end() && "Component added to same entity more than once.");

		size_t new_index = m_Size;
		m_EntityToIndexMap[entity] = new_index;
		m_IndexToEntityMap[new_index] = entity;
		m_ComponentArray[new_index] = component;
		++m_Size;
	}

	void RemoveData(Entity entity)
	{
		assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end() && "Removing non-existent component.");

		size_t removed_entity_index = m_EntityToIndexMap[entity];
		size_t last_element_index = m_Size - 1;
		m_ComponentArray[removed_entity_index] = m_ComponentArray[last_element_index];

		Entity last_entity = m_IndexToEntityMap[last_element_index];
		m_EntityToIndexMap[last_entity] = removed_entity_index;
		m_IndexToEntityMap[removed_entity_index] = last_entity;

		m_EntityToIndexMap.erase(entity);
		m_IndexToEntityMap.erase(last_element_index);

		--m_Size;
	}

	T& GetData(Entity entity)
	{
		assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end() && "Retrieving non-existent component.");

		return m_ComponentArray[m_EntityToIndexMap[entity]];
	}

	void EntityDestroyed(Entity entity) override
	{
		if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end())
		{
			RemoveData(entity);
		}
	}

private:
	size_t m_Size;
	std::array<T, MAX_ENTITIES> m_ComponentArray;
	std::unordered_map<Entity, size_t> m_EntityToIndexMap;
	std::unordered_map<size_t, Entity> m_IndexToEntityMap;
};

class ComponentManager
{
public:
	template<typename T>
	void RegisterComponent()
	{
		size_t hash = typeid(T).hash_code();
		assert(m_ComponentTypes.find(hash) == m_ComponentTypes.end() && "Registering component type more than once.");

		m_ComponentTypes.insert({ hash, m_NextComponentType });
		m_ComponentArrays.insert({ hash, std::make_shared<ComponentArray<T>>() });

		++m_NextComponentType;
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		size_t hash = typeid(T).hash_code();
		assert(m_ComponentTypes.find(hash) != m_ComponentTypes.end() && "Component not registered before use.");

		return m_ComponentTypes[hash];
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		size_t hash = typeid(T).hash_code();
		assert(m_ComponentTypes.find(hash) != m_ComponentTypes.end() && "Component not registered before use.");

		GetComponentArray<T>()->InsertData(entity, component);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
	T& GetComponenet(Entity entity)
	{
		return GetComponentArray<T>()->GetData(entity);
	}

	void EntityDestroyed(Entity entity)
	{
		for (const std::pair<size_t, std::shared_ptr<IComponentArray>>& pair : m_ComponentArrays)
		{
			const std::shared_ptr<IComponentArray>& component = pair.second;
			component->EntityDestroyed(entity);
		}
	}

private:
	std::unordered_map<size_t, ComponentType> m_ComponentTypes{};
	std::unordered_map<size_t, std::shared_ptr<IComponentArray>> m_ComponentArrays{};
	ComponentType m_NextComponentType{};

	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		size_t hash = typeid(T).hash_code();

		assert(m_ComponentTypes.find(hash) != m_ComponentTypes.end() && "Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[hash]);
	}
};

class SystemManager
{
public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		size_t hash = typeid(T).hash_code();
		assert(m_Systems.find(hash) == m_Systems.end() && "Registering system more than once.");

		std::shared_ptr<T> system = std::make_shared<T>();
		m_Systems.insert({ hash, system });
		return system;
	}

	template<typename T>
	void SetSignature(Signature signature)
	{
		size_t hash = typeid(T).hash_code();
		assert(m_Systems.find(hash) != m_Systems.end() && "System used before registered.");

		m_Signatures.insert({ hash, signature });
	}

	void EntityDestroyed(Entity entity)
	{
		for (const std::pair<size_t, std::shared_ptr<IEntitySystem>>& pair : m_Systems)
		{
			const std::shared_ptr<IEntitySystem>& system = pair.second;
			system->m_Entities.erase(entity);
		}
	}

	void EntitySignatureChanged(Entity entity, Signature signature)
	{
		for (const std::pair<const size_t, std::shared_ptr<IEntitySystem>>& pair : m_Systems)
		{
			const size_t& type = pair.first;
			const std::shared_ptr<IEntitySystem>& system = pair.second;
			const Signature& systemSignature = m_Signatures[type];

			if ((signature & systemSignature) == systemSignature)
			{
				system->m_Entities.insert(entity);
			}
			else
			{
				system->m_Entities.erase(entity);
			}
		}
	}

	template<typename T>
	std::shared_ptr<T> GetSystem()
	{
		size_t hash = typeid(T).hash_code();
		assert(m_Systems.find(hash) != m_Systems.end() && "System type not registered.");

		return std::static_pointer_cast<T>(m_Systems[hash]);
	}

	std::vector<std::shared_ptr<IEntitySystem>> GetSystems()
	{
		std::vector<std::shared_ptr<IEntitySystem>> systems;
		for (const auto& system : m_Systems)
		{
			systems.push_back(system.second);
		}
		return systems;
	}

private:
	std::unordered_map<size_t, Signature> m_Signatures{};
	std::unordered_map<size_t, std::shared_ptr<IEntitySystem>> m_Systems{};
};

class EntityManager
{
public:
	EntityManager()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			m_EntityPool.push(entity);
		}

		m_ComponentManager = std::make_unique<ComponentManager>();
		m_SystemManager = std::make_unique<SystemManager>();
	}

	Entity CreateEntity()
	{
		Entity entity = m_EntityPool.front();
		m_EntityPool.pop();

		++m_ActiveEntities;

		return entity;
	}

	void DestroyEntity(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		m_Signatures[entity].reset();
		m_EntityPool.push(entity);
		--m_ActiveEntities;

		m_ComponentManager->EntityDestroyed(entity);
		m_SystemManager->EntityDestroyed(entity);
	}

	void SetSignature(Entity entity, Signature signature)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		m_Signatures[entity] = signature;
	}

	Signature& GetSignature(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");
		return m_Signatures[entity];
	}

	template<typename T>
	void RegisterComponent()
	{
		m_ComponentManager->RegisterComponent<T>();
	}

	template<typename T>
	T& AddComponent(Entity entity, T component)
	{
		m_ComponentManager->AddComponent<T>(entity, component);

		Signature signature = GetSignature(entity);
		signature.set(m_ComponentManager->GetComponentType<T>(), true);
		SetSignature(entity, signature);

		m_SystemManager->EntitySignatureChanged(entity, signature);

		return m_ComponentManager->GetComponenet<T>(entity);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		m_ComponentManager->RemoveComponent<T>(entity);

		Signature signature = GetSignature(entity);
		signature.set(m_ComponentManager->GetComponentType<T>(), false);
		SetSignature(entity, signature);

		m_SystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		return m_ComponentManager->GetComponenet<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return m_ComponentManager->GetComponentType<T>();
	}

	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return m_SystemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemSignature(Signature signature)
	{
		m_SystemManager->SetSignature<T>(signature);
	}

	template<typename T>
	std::shared_ptr<T> GetSystem()
	{
		return m_SystemManager->GetSystem<T>();
	}

	std::vector<std::shared_ptr<IEntitySystem>> GetSystems()
	{
		return m_SystemManager->GetSystems();
	}

	std::vector<Entity> GetEntities()
	{
		std::vector<Entity> entities;
		for (Entity entity = 0; entity < m_ActiveEntities; ++entity)
		{
			entities.push_back(entity);
		}
		return entities;
	}

	Entity GetEntityFromEndpoint(const asio::ip::udp::endpoint& remote_endpoint)
	{
		for (Entity entity = 0; entity < m_ActiveEntities; ++entity)
		{
			const Connection& client = GetComponent<Connection>(entity);
			if (client.m_Endpoint == remote_endpoint)
			{
				return entity;
			}
		}

		return -1;
	}

private:
	std::queue<Entity> m_EntityPool{};
	std::array<std::bitset<MAX_COMPONENTS>, MAX_ENTITIES> m_Signatures{};
	Entity m_ActiveEntities{};
	std::unique_ptr<ComponentManager> m_ComponentManager;
	std::unique_ptr<SystemManager> m_SystemManager;
};