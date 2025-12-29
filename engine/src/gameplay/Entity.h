#pragma once

// external
#include <string>

// gameplay
#include "gameplay/EntityID.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Entity
		//---------------------------------------------------------------------
		/// <summary>
		/// Wrapper class that contains specific info all entities have.
		/// </summary>
		class Entity
		{
		public:
			/// <summary>
			/// Constructs a default entity with no ID and an empty name.
			/// </summary>
			Entity() = default;

			/// <summary>
			/// Constructs an entity with a specific ID and name.
			/// </summary>
			/// <param name="a_EntityID">The unique identifier for this entity.</param>
			/// <param name="a_sName">The name of the entity.</param>
			Entity(const EntityID& a_EntityID, const std::string& a_sName) :
				m_EntityID(a_EntityID),
				m_sName(a_sName)
			{}

			/// <summary>
			/// Gets a reference to the entity�s unique identifier.
			/// </summary>
			/// <returns>Reference to the entity ID.</returns>
			EntityID& GetEntityID()
			{
				return m_EntityID;
			}

			/// <summary>
			/// Gets a constant reference to the entity�s unique identifier.
			/// </summary>
			/// <returns>Constant reference to the entity ID.</returns>
			const EntityID& GetEntityID() const
			{
				return m_EntityID;
			}

			/// <summary>
			/// Sets the parent of the entity.
			/// </summary>
			/// <param name="a_EntityID">The parent of the entity.</param>
			void SetParent(const EntityID& a_EntityID)
			{
				if (m_EntityID == m_ParentID)
				{
					return;
				}
				m_ParentID = a_EntityID;
			}

			/// <summary>
			/// Clears the parent of the entity.
			/// </summary>
			void Clear()
			{
				m_ParentID.SetInvalid();
			}

			/// <summary>
			/// Gets a reference to the entity�s unique identifier.
			/// </summary>
			/// <returns>Reference to the entity ID.</returns>
			EntityID& GetParent()
			{
				return m_ParentID;
			}

			/// <summary>
			/// Gets a constant reference to the entity�s unique identifier.
			/// </summary>
			/// <returns>Constant reference to the entity ID.</returns>
			const EntityID& GetParent() const
			{
				return m_ParentID;
			}

			/// <summary>
			/// Sets the name of the entity.
			/// </summary>
			/// <param name="a_sName">The new name of the entity.</param>
			void SetName(const std::string& a_sName)
			{
				m_sName = a_sName;
			}

			/// <summary>
			/// Gets the name of the entity.
			/// </summary>
			/// <returns>Reference to the entity name string.</returns>
			const std::string& GetName() const
			{
				return m_sName;
			}

			/// <summary>
			/// Checks if the entity has been destroyed.
			/// </summary>
			/// <returns>True if the entity is destroyed, false otherwise.</returns>
			bool IsDestroyed() const
			{
				return m_bIsDestroyed;
			}

			/// <summary>
			/// Destroys the entity and all its children.
			/// </summary>
			void Destroy();

			/// <summary>
			/// Marks the entity as destroyed.
			/// </summary>
			void SetDestroy();

			/// <summary>
			/// Checks if the entity is currently active.
			/// Inactive entities are not updated or rendered.
			/// </summary>
			/// <returns>True if the entity is active, false otherwise.</returns>
			bool IsActive() const
			{
				return m_bIsActive;
			}

			/// <summary>
			/// Sets whether the entity is active.
			/// </summary>
			/// <param name="a_bIsActive">True to activate, false to deactivate.</param>
			void SetIsActive(bool a_bIsActive)
			{
				m_bIsActive = a_bIsActive;
			}
		private:
			EntityID m_EntityID;
			EntityID m_ParentID;
			std::string m_sName;
			bool m_bIsDestroyed = false;
			bool m_bIsActive = true;
		};
	}
}