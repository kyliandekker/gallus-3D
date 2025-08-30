#pragma once

#include "core/System.h"

// std::is_base_of
#include <type_traits> 
#include <map>
#include <string>
#include <vector>

#include "gameplay/EntityID.h"
#include "gameplay/systems/components/Component.h"
#include "core/Tool.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// AbstractECSSystem
		//---------------------------------------------------------------------
		class AbstractECSSystem : public core::System
		{
		public:
			virtual ~AbstractECSSystem() = default;
			AbstractECSSystem() = default;

			/// <summary>
			/// Retrieves the property name of the system (used in serialization).
			/// </summary>
			/// <returns>A string containing the property name of the system.</returns>
			virtual std::string GetPropertyName() const = 0;

			/// <summary>
			/// Retrieves the name of the system.
			/// </summary>
			/// <returns>A string containing the name of the system.</returns>
			virtual std::string GetSystemName() const = 0;

			/// <summary>
			/// Clears the system and removes all entities.
			/// </summary>
			virtual void Clear() = 0;

			/// <summary>
			/// Deletes a component from the system.
			/// </summary>
			/// <param name="a_ID">The entity ID that needs to get deleted.</param>
			virtual void DeleteComponent(const EntityID& a_ID) = 0;

			/// <summary>
			/// Updates the system.
			/// </summary>
			/// <param name="a_fDeltaTime">The time that has passed since the last frame.</param>
			virtual void Update(float a_fDeltaTime) = 0;

			/// <summary>
			/// Updates the components in the system.
			/// </summary>
			virtual void UpdateComponents() = 0;

			/// <summary>
			/// Checks whether an entity is using the system.
			/// </summary>
			/// <param name="a_ID">The entity that will be checked.</param>
			/// <returns>True if the entity existed, otherwise false.</returns>
			virtual bool HasComponent(const EntityID& a_ID) = 0;

			/// <summary>
			/// Retrieves a component by entity id.
			/// </summary>
			/// <param name="a_ID">The entity that will be checked.</param>
			/// <returns>Pointer to the component if the entity existed, otherwise nullptr.</returns>
			virtual Component* GetBaseComponent(const EntityID& a_ID) = 0;

			/// <summary>
			/// Creates a component in the entity component system.
			/// </summary>
			/// <param name="a_ID"></param>
			virtual Component* CreateBaseComponent(const EntityID& a_ID) = 0;
		};

		//---------------------------------------------------------------------
		// ECSBaseSystem
		//---------------------------------------------------------------------
		template <class ComponentType>
		class ECSBaseSystem : public AbstractECSSystem
		{
			static_assert(std::is_base_of<Component, ComponentType>::value,
				"ComponentType must be derived from Component");
		public:
			/// <summary>
			/// Destroys the system, releasing resources and performing necessary cleanup.
			/// </summary>
			/// <returns>True if the destruction was successful, otherwise false.</returns>
			bool Destroy() override
			{
				return AbstractECSSystem::Destroy();
			}

			virtual ~ECSBaseSystem() = default;

			/// <summary>
			/// Creates a component in the entity component system.
			/// </summary>
			/// <param name="a_ID"></param>
			Component* CreateBaseComponent(const EntityID& a_ID) override
			{
				if (!HasComponent(a_ID))
				{
					ComponentType t;
					m_mComponents.insert(std::make_pair(a_ID, t));
				}
				core::TOOL->GetECS().OnEntityComponentsUpdated().invoke();
				Component& comp = m_mComponents.at(a_ID);
				comp.Init();
				return &comp;
			}

			/// <summary>
			/// Retrieves the number of entities using this system.
			/// </summary>
			/// <returns>Number representing the amount of entities that use this system.</returns>
			size_t GetSize() const
			{
				return m_mComponents.size();
			}

			/// <summary>
			/// Checks whether an entity is using the system.
			/// </summary>
			/// <param name="a_ID">The entity that will be checked.</param>
			/// <returns>True if the entity existed, otherwise false.</returns>
			bool HasComponent(const EntityID& a_ID)
			{
				return m_mComponents.count(a_ID) == 1;
			}

			/// <summary>
			/// Retrieves a component by entity id.
			/// </summary>
			/// <param name="a_ID">The entity that will be checked.</param>
			/// <returns>Reference to the component if the entity existed, otherwise it gets created and returns that.</returns>
			ComponentType& GetComponent(const EntityID& a_ID)
			{
				if (HasComponent(a_ID))
				{
					return m_mComponents.at(a_ID);
				}

				CreateBaseComponent(a_ID);
				return m_mComponents.at(a_ID);
			}

			/// <summary>
			/// Deletes a component from the system.
			/// </summary>
			/// <param name="a_ID">The entity ID that needs to get deleted.</param>
			void DeleteComponent(const EntityID& a_ID)
			{
				auto it = std::find_if(m_mComponents.begin(), m_mComponents.end(), [&](auto& e)
					{
						return e.first == a_ID;
					});
				if (it != m_mComponents.end())
				{
					it->second.Destroy();
				}
			}

			/// <summary>
			/// Clears the system and removes all entities.
			/// </summary>
			void Clear() override
			{
				m_mComponents.clear();
			}

			/// <summary>
			/// Retrieves a component by entity id.
			/// </summary>
			/// <param name="a_ID">The entity that will be checked.</param>
			/// <returns>Pointer to the component if the entity existed, otherwise nullptr.</returns>
			Component* GetBaseComponent(const EntityID& a_ID) override
			{
				ComponentType& comp = GetComponent(a_ID);
				return &comp;
			};

			/// <summary>
			/// Updates the system's components.
			/// </summary>
			virtual void UpdateComponents() override
			{
				size_t oldSize = m_mComponents.size();
				std::erase_if(m_mComponents, [](auto& pair)
					{
						return pair.second.IsDestroyed();
					});

				if (oldSize != m_mComponents.size())
				{
					core::TOOL->GetECS().OnEntityComponentsUpdated().invoke();
				}
			}

			/// <summary>
			/// Retrieves all mesh components.
			/// </summary>
			/// <returns>A vector containing the entity info and component data of all entities.</returns>
			std::map<EntityID, ComponentType>& GetComponents()
			{
				return m_mComponents;
			}
		protected:
			// TODO: We can only have one for each entity. If I want multiple components this will be a problem.
			std::map<EntityID, ComponentType> m_mComponents;
		};
	}
}