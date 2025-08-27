#include "gameplay/EntityComponentSystem.h"

#include "logger/Logger.h"

#include "gameplay/ECSBaseSystem.h"

#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/SpriteSystem.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// EntityComponentSystem
		//---------------------------------------------------------------------
		bool EntityComponentSystem::Initialize()
		{
			// These systems are in order of initialization.
			CreateSystem<TransformSystem>();
			CreateSystem<SpriteSystem>();

			for (AbstractECSSystem* system : m_aSystems)
			{
				system->Initialize();
			}

			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ECS, "ECS initialized.");
			return System::Initialize();
		}

		//---------------------------------------------------------------------
		bool EntityComponentSystem::Destroy()
		{
			for (AbstractECSSystem* system : m_aSystems)
			{
				system->Destroy();
				delete system;
			}
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ECS, "ECS destroyed.");
			return System::Destroy();
		}

		//---------------------------------------------------------------------
		void EntityComponentSystem::Update(const float& a_fDeltaTime)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			for (const Entity& entity : m_aEntities)
			{
				for (AbstractECSSystem* sys : m_aSystems)
				{
					if (entity.IsDestroyed())
					{
						sys->DeleteComponent(entity.GetEntityID());
					}
				}
			}

			size_t oldSize = m_aEntities.size();
			m_aEntities.erase(
				std::remove_if(m_aEntities.begin(), m_aEntities.end(),
				[](const Entity& e)
				{
					return e.IsDestroyed();
				}),
				m_aEntities.end()
			);

			if (oldSize != m_aEntities.size())
			{
				m_eOnEntitiesUpdated();
				m_eOnEntityComponentsUpdated();
			}

			// Delete components if applicable.
			for (auto& sys : m_aSystems)
			{
				sys->UpdateComponents();
			}

			if (!m_bStarted)
			{
				return;
			}

			if (m_bPaused)
			{
				return;
			}

			for (auto& sys : m_aSystems)
			{
				sys->Update(a_fDeltaTime);
			}

		}

		//---------------------------------------------------------------------
		bool EntityComponentSystem::IsPaused() const
		{
			return m_bPaused;
		}

		//---------------------------------------------------------------------
		void EntityComponentSystem::SetPaused(bool a_bPaused)
		{
			m_bPaused = a_bPaused;
		}

		//---------------------------------------------------------------------
		bool EntityComponentSystem::HasStarted() const
		{
			return m_bStarted;
		}

		//---------------------------------------------------------------------
		void EntityComponentSystem::SetStarted(bool a_bStarted)
		{
			m_bStarted = a_bStarted;
		}

		//---------------------------------------------------------------------
		EntityID EntityComponentSystem::CreateEntity(const std::string& a_sName)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			const EntityID id(++m_iNextID);
			m_aEntities.emplace_back(id, a_sName);

			m_eOnEntitiesUpdated();

			return id;
		}

		//---------------------------------------------------------------------
		bool EntityComponentSystem::IsEntityValid(const EntityID& a_ID) const
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			return a_ID.IsValid();
		}

		//---------------------------------------------------------------------
		void EntityComponentSystem::DeleteEntity(const EntityID& a_ID)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			auto it = std::find_if(m_aEntities.begin(), m_aEntities.end(), [&](Entity& e)
				{
					return e.GetEntityID() == a_ID;
				});
			if (it != m_aEntities.end())
			{
				it->Destroy();
			}
		}

		//---------------------------------------------------------------------
		const Entity* EntityComponentSystem::GetEntity(const EntityID& a_ID) const
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			auto it = std::find_if(m_aEntities.begin(), m_aEntities.end(),
				[&](const Entity& e)
				{
					return e.GetEntityID() == a_ID;
				});

			if (it != m_aEntities.end())
			{
				return &(*it);
			}

			return nullptr;
		}

		//---------------------------------------------------------------------
		Entity* EntityComponentSystem::GetEntity(const EntityID& a_ID)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			auto it = std::find_if(m_aEntities.begin(), m_aEntities.end(), [&](Entity& e)
				{
					return e.GetEntityID() == a_ID;
				});

			if (it != m_aEntities.end())
				return &(*it);

			return nullptr;
		}

		//---------------------------------------------------------------------
		void EntityComponentSystem::Clear()
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			for (Entity& entity : m_aEntities)
			{
				entity.Destroy();
			}
		}

		//---------------------------------------------------------------------
		std::string EntityComponentSystem::GetUniqueName(const std::string& a_sName)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			std::string name = a_sName;

			bool found = true;
			int i = 0;
			while (found)
			{
				found = false;
				for (Entity& entity : m_aEntities)
				{
					if (entity.GetName() == name)
					{
						i++;
						if (i != 0)
						{
							name = a_sName + " (" + std::to_string(i) + ")";
							found = true;
						}
					}
				}
			}
			return name;
		}

		//---------------------------------------------------------------------
		std::vector<Entity>& EntityComponentSystem::GetEntities()
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);
			return m_aEntities;
		}

		//---------------------------------------------------------------------
		std::vector<AbstractECSSystem*> EntityComponentSystem::GetSystemsContainingEntity(const EntityID& a_ID)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			std::vector<AbstractECSSystem*> systems;
			for (AbstractECSSystem* system : m_aSystems)
			{
				if (system->HasComponent(a_ID))
				{
					systems.push_back(system);
				}
			}
			return systems;
		}

		//---------------------------------------------------------------------
		std::vector<AbstractECSSystem*> EntityComponentSystem::GetSystemsContainingEntity(const Entity& a_Entity)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			return GetSystemsContainingEntity(a_Entity.GetEntityID());
		}

		//---------------------------------------------------------------------
		std::vector<AbstractECSSystem*> EntityComponentSystem::GetSystems()
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			return m_aSystems;
		}
	}
}