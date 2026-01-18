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
			m_aSystems.clear();
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ECS, "ECS destroyed.");
			return System::Destroy();
		}

		bool previousUpdateRealtimeState = false;
		//---------------------------------------------------------------------
		void EntityComponentSystem::Update(float a_fDeltaTime, bool a_bUpdateRealtime)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			// Update components.
			for (std::unique_ptr<AbstractECSSystem>& sys : m_aSystems)
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
			uint32_t index;
			if (!m_aFreeIndices.empty())
			{
				index = m_aFreeIndices.back();
				m_aFreeIndices.pop_back();
			}
			else
			{
				index = static_cast<uint32_t>(m_aEntitySlots.size());
				m_aEntitySlots.emplace_back();
			}

			EntitySlot& slot = m_aEntitySlots[index];
			slot.m_pEntity = std::make_shared<Entity>(EntityID(index, slot.m_iGeneration), a_sName);

			m_eOnEntitiesUpdated.invoke();
			m_eOnEntityComponentsUpdated.invoke();

			return EntityID(index, slot.m_iGeneration);
		}

		//---------------------------------------------------------------------
		bool EntityComponentSystem::IsEntityValid(const EntityID& a_ID) const
		{
			if (!a_ID.IsValid())
			{
				return false;
			}

			if (a_ID.GetIndex() >= m_aEntitySlots.size())
			{
				return false;
			}

			const EntitySlot& slot = m_aEntitySlots[a_ID.GetIndex()];
			return slot.m_iGeneration == a_ID.GetGeneration() &&
				slot.m_pEntity != nullptr;
		}

		//---------------------------------------------------------------------
		void EntityComponentSystem::DeleteEntity(const EntityID& a_ID)
		{
			if (!IsEntityValid(a_ID))
			{
				return;
			}

			EntitySlot& slot = m_aEntitySlots[a_ID.GetIndex()];
			slot.m_pEntity.reset();
			slot.m_iGeneration++;
			m_aFreeIndices.push_back(a_ID.GetIndex());

			m_eOnEntitiesUpdated.invoke();
			m_eOnEntityComponentsUpdated.invoke();
		}

		//---------------------------------------------------------------------
		std::weak_ptr<Entity> EntityComponentSystem::GetEntity(const EntityID& a_ID) const
		{
			if (!IsEntityValid(a_ID))
			{
				return std::weak_ptr<Entity>();
			}

			return m_aEntitySlots[a_ID.GetIndex()].m_pEntity;
		}

		//---------------------------------------------------------------------
		std::weak_ptr<Entity> EntityComponentSystem::GetEntityByName(const std::string& a_sName) const
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			for (const EntitySlot& slot : m_aEntitySlots)
			{
				if (!slot.m_pEntity)
				{
					continue;
				}

				if (slot.m_pEntity->GetName() == a_sName)
				{
					return slot.m_pEntity;
				}
			}

			return std::weak_ptr<Entity>();
		}

		//---------------------------------------------------------------------
		std::weak_ptr<Entity> EntityComponentSystem::GetEntity(const EntityID& a_ID)
		{
			return static_cast<const EntityComponentSystem*>(this)->GetEntity(a_ID);
		}

		//---------------------------------------------------------------------
		void EntityComponentSystem::Clear()
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			m_aFreeIndices.clear();

			for (uint32_t i = 0; i < m_aEntitySlots.size(); ++i)
			{
				EntitySlot& slot = m_aEntitySlots[i];

				if (slot.m_pEntity)
				{
					// If component systems listen to entity destruction, call DeleteComponent here.
					for (std::unique_ptr<AbstractECSSystem>& sys : m_aSystems)
					{
						sys->DeleteComponent(EntityID(i, slot.m_iGeneration));
					}

					slot.m_pEntity.reset();
				}

				slot.m_iGeneration++;          // Invalidate old IDs
				m_aFreeIndices.push_back(i);   // Mark slot as free
			}

			m_eOnEntitiesUpdated.invoke();
			m_eOnEntityComponentsUpdated.invoke();
		}

		//---------------------------------------------------------------------
		std::string EntityComponentSystem::GetUniqueName(const std::string& a_sName)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			std::string candidate = a_sName;
			uint32_t counter = 1;
			bool exists = true;

			while (exists)
			{
				exists = false;

				for (const EntitySlot& slot : m_aEntitySlots)
				{
					if (!slot.m_pEntity)
					{
						continue;
					}

					if (slot.m_pEntity->GetName() == candidate)
					{
						candidate = a_sName + " (" + std::to_string(counter++) + ")";
						exists = true;
						break;
					}
				}
			}

			return candidate;
		}

		//---------------------------------------------------------------------
		std::vector<EntityID> EntityComponentSystem::GetEntities() const
		{
			std::vector<EntityID> result;
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			for (uint32_t i = 0; i < m_aEntitySlots.size(); ++i)
			{
				const EntitySlot& slot = m_aEntitySlots[i];
				if (slot.m_pEntity)
				{
					result.emplace_back(i, slot.m_iGeneration);
				}
			}

			return result;
		}

		//---------------------------------------------------------------------
		std::vector<AbstractECSSystem*> EntityComponentSystem::GetSystemsContainingEntity(const EntityID& a_ID)
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			std::vector<AbstractECSSystem*> systems;
			for (std::unique_ptr<AbstractECSSystem>& sys : m_aSystems)
			{
				if (sys->HasComponent(a_ID))
				{
					systems.push_back(sys.get());
				}
			}
			return systems;
		}

		//---------------------------------------------------------------------
		std::vector<AbstractECSSystem*> EntityComponentSystem::GetSystems()
		{
			std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

			std::vector<AbstractECSSystem*> systems;
			for (std::unique_ptr<AbstractECSSystem>& sys : m_aSystems)
			{
				systems.push_back(sys.get());
			}
			return systems;
		}
	}
}