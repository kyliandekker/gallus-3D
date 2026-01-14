#pragma once

#include "core/System.h"

// external
#include <vector>
#include <string>
#include <mutex>
#include <memory>

// core
#include "core/Event.h"

// gameplay
#include "gameplay/Entity.h"

namespace gallus
{
	namespace gameplay
	{
		class AbstractECSSystem;

		/// <summary>
		/// Class that contains all gameplay elements in the engine.
		/// </summary>
		//---------------------------------------------------------------------
		// EntityComponentSystem
		//---------------------------------------------------------------------
		class EntityComponentSystem : public core::System
		{
		public:
			/// <summary>
			/// Initializes the system, setting up necessary resources.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			bool Initialize() override;

			/// <summary>
			/// Destroys the system, releasing resources and performing necessary cleanup.
			/// </summary>
			/// <returns>True if the destruction was successful, otherwise false.</returns>
			bool Destroy() override;

			/// <summary>
			/// Updates the system.
			/// </summary>
			void Update(float a_fDeltaTime, bool a_bUpdateRealtime = false);

			/// <summary>
			/// Creates an entity.
			/// </summary>
			/// <param name="a_sName">The name of the entity.</param>
			/// <returns>The entity ID that got created.</returns>
			EntityID CreateEntity(const std::string& a_sName);

			/// <summary>
			/// Checks whether an entity is valid.
			/// </summary>
			/// <param name="a_ID"></param>
			/// <returns>True if the entity was valid, otherwise false.</returns>
			bool IsEntityValid(const EntityID& a_ID) const;

			/// <summary>
			/// Deletes an entity.
			/// </summary>
			/// <param name="a_ID">The entity that will be deleted.</param>
			void DeleteEntity(const EntityID& a_ID);

			/// <summary>
			/// Gets entity info from a specific entity.
			/// </summary>
			/// <param name="a_ID">The entity.</param>
			std::weak_ptr<Entity> GetEntity(const EntityID& a_ID) const
			{
				std::lock_guard<std::recursive_mutex> lock(m_EntityMutex);

				for (const std::shared_ptr<Entity>& ent : m_aEntities)
				{
					if (ent->GetEntityID() == a_ID)
					{
						return ent;
					}
				}

				return std::weak_ptr<Entity>();
			}

			/// <summary>
			/// Gets entity info from a specific entity.
			/// </summary>
			std::weak_ptr<Entity> GetEntityByName(const std::string& a_sName) const;

			/// <summary>
			/// Gets entity info from a specific entity.
			/// </summary>
			/// <param name="a_ID">The entity.</param>
			std::weak_ptr<Entity> GetEntity(const EntityID& a_ID)
			{
				return static_cast<const EntityComponentSystem*>(this)->GetEntity(a_ID);
			}

			/// <summary>
			/// Clears all entities and deletes all components.
			/// </summary>
			void Clear();

			/// <summary>
			/// Returns a unique name and looks through all entities if a name already exists.
			/// </summary>
			/// <param name="a_Name">The name to check against.</param>
			/// <returns>A string containing a unique name for an entity.</returns>
			std::string GetUniqueName(const std::string& a_Name);

			/// <summary>
			/// Creates a system in the ECS.
			/// </summary>
			/// <typeparam name="T">The system class.</typeparam>
			/// <returns>A reference to the created system.</returns>
			template <class T>
			T& CreateSystem()
			{
				for (AbstractECSSystem* sys : m_aSystems)
				{
					T* result = dynamic_cast<T*>(sys);
					if (result)
					{
						return *result;
					}
				}
				T* system = new T();
				m_aSystems.push_back(system);
				return *system;
			}

			/// <summary>
			/// Retrieves a system from the ECS. Creates one if not present.
			/// </summary>
			/// <typeparam name="T">The system class.</typeparam>
			/// <returns>A reference to the created system.</returns>
			template <class T>
			T& GetSystem()
			{
				for (AbstractECSSystem* sys : m_aSystems)
				{
					T* result = dynamic_cast<T*>(sys);
					if (result)
					{
						return *result;
					}
				}
				return CreateSystem<T>();
			};

			/// <summary>
			/// Retrieves all entities in the ECS.
			/// </summary>
			/// <returns>A vector containing all entities in the ECS.</returns>
			std::vector<std::weak_ptr<Entity>> GetEntities();

			/// <summary>
			/// Retrieves all systems that have components for the specified entity id.
			/// </summary>
			/// <param name="a_EntityID">The entity that will be checked.</param>
			/// <returns>A vector containing all systems that have components for the entity id.</returns>
			std::vector<AbstractECSSystem*> GetSystemsContainingEntity(const EntityID& a_EntityID);

			/// <summary>
			/// Retrieves all systems in the ECS.
			/// </summary>
			/// <returns>A vector containing all systems in the ECS.</returns>
			std::vector<AbstractECSSystem*> GetSystems();

			mutable std::recursive_mutex m_EntityMutex;

			/// <summary>
			/// Retrieves the event that gets called after any entity has been updated.
			/// </summary>
			/// <returns>A const reference to the event.</returns>
			const Event<>& OnEntitiesUpdated() const
			{
				return m_eOnEntitiesUpdated;
			}

			/// <summary>
			/// Retrieves the event that gets called after any entity component has been updated.
			/// </summary>
			/// <returns>A const reference to the event.</returns>
			const Event<>& OnEntityComponentsUpdated() const
			{
				return m_eOnEntityComponentsUpdated;
			}
		private:
			Event<> m_eOnEntitiesUpdated;
			Event<> m_eOnEntityComponentsUpdated;

			std::vector<AbstractECSSystem*> m_aSystems;
			std::vector<std::shared_ptr<Entity>> m_aEntities;
			size_t m_iNextID = 0;
		};
	}
}