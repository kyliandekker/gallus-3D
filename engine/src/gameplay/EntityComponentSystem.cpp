#include "EntityComponentSystem.h"

// logger
#include "logger/Logger.h"

// gameplay
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
			m_aSystems.clear();
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ECS, "ECS destroyed.");
			return System::Destroy();
		}

		bool previousUpdateRealtimeState = false;
		//---------------------------------------------------------------------
		void EntityComponentSystem::Update(float a_fDeltaTime, bool a_bUpdateRealtime)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			for (const std::weak_ptr<Entity> entity : m_aEntities)
			{
				if (auto ent = entity.lock())
				{
					for (AbstractECSSystem* sys : m_aSystems)
					{
						if (ent->IsDestroyed())
						{
							sys->DeleteComponent(ent->GetEntityID());
						}
					}
				}
			}

			size_t oldSize = m_aEntities.size();
			m_aEntities.erase(
				std::remove_if(m_aEntities.begin(), m_aEntities.end(),
				[](std::shared_ptr<Entity> e)
				{
					return e->IsDestroyed();
				}),
				m_aEntities.end()
			);

			if (oldSize != m_aEntities.size())
			{
				m_eOnEntitiesUpdated();
				m_eOnEntityComponentsUpdated();
			}

			for (auto& sys : m_aSystems)
			{
				sys->UpdateComponents();
			}

			if (a_bUpdateRealtime)
			{
				for (auto& sys : m_aSystems)
				{
					sys->InitComponentsRealtime();
				}
				for (uint32_t i = 1; i <= static_cast<uint32_t>(UpdateTime::UPDATE_TIME_FRAME_END); i <<= 1)
				{
					UpdateTime updateTime = (UpdateTime)i;
					for (auto& sys : m_aSystems)
					{
						if (sys->GetUpdateTimes().HasFlag(updateTime))
						{
							sys->UpdateComponentsRealtime(a_fDeltaTime, updateTime);
						}
					}
				}
			}
			previousUpdateRealtimeState = a_bUpdateRealtime;
		}

		//---------------------------------------------------------------------
		EntityID EntityComponentSystem::CreateEntity(const std::string& a_sName)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			const EntityID id(++m_iNextID);
			m_aEntities.push_back(std::make_shared<Entity>(id, a_sName));
			
			m_eOnEntitiesUpdated();
			m_eOnEntityComponentsUpdated();

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

			for (const std::shared_ptr<Entity>& ent : m_aEntities)
			{
				if (ent->GetEntityID() == a_ID)
				{
					ent->Destroy();
					return;
				}
			}
		}

		//---------------------------------------------------------------------
		std::weak_ptr<Entity> EntityComponentSystem::GetEntityByName(const std::string& a_sName) const
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			for (const std::shared_ptr<Entity>& ent : m_aEntities)
			{
				if (ent->GetName() == a_sName)
				{
					return ent;
				}
			}

			return std::weak_ptr<Entity>();
		}

		//---------------------------------------------------------------------
		void EntityComponentSystem::Clear()
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			for (std::weak_ptr<Entity> entity : m_aEntities)
			{
				if (auto ent = entity.lock())
				{
					ent->Destroy();
				}
			}
		}

		//---------------------------------------------------------------------
		std::string EntityComponentSystem::GetUniqueName(const std::string& a_sName)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			std::string name = a_sName;

			bool found = true;
			size_t i = 0;
			while (found)
			{
				found = false;
				for (std::weak_ptr<Entity> entity : m_aEntities)
				{
					if (auto ent = entity.lock())
					{
						if (ent->GetName() == name)
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
			}
			return name;
		}

		//---------------------------------------------------------------------
		std::vector<std::weak_ptr<Entity>> EntityComponentSystem::GetEntities()
		{
			std::vector<std::weak_ptr<Entity>> entities;
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);
			for (auto sharedPtr : m_aEntities)
			{
				entities.push_back(sharedPtr);
			}
			return entities;
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
		std::vector<AbstractECSSystem*> EntityComponentSystem::GetSystems()
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			return m_aSystems;
		}
	}
}