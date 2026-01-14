#pragma once

// external
#include <string>

namespace gallus
{
	namespace gameplay
	{
		class TransformComponent;

		//---------------------------------------------------------------------
		// EntityID
		//---------------------------------------------------------------------
		struct EntityID
		{
			/// <summary>
			/// Constructs an empty entity ID.
			/// </summary>
			EntityID() = default;

			/// <summary>
			/// Constructs an entity with a given ID.
			/// </summary>
			/// <param name="a_iID">The id.</param>
			EntityID(size_t a_iID) : m_iID(a_iID)
			{};

			/// <summary>
			/// Deconstructs an entity ID.
			/// </summary>
			~EntityID() = default;

			/// <summary>
			/// Checks whether the entity is valid or not.
			/// </summary>
			/// <returns>True if the entity was valid, otherwise false.</returns>
			bool IsValid() const
			{
				return m_iID != INVALID;
			};

			/// <summary>
			/// Sets the entity to invalid.
			/// </summary>
			void SetInvalid()
			{
				m_iID = INVALID;
			}

			/// <summary>
			/// Retrieves the ID (as an integer).
			/// </summary>
			/// <returns>An integer containing the entity id.</returns>
			size_t GetID() const
			{
				return m_iID;
			}

			bool operator==(const EntityID& a_Other) const
			{
				return m_iID == a_Other.m_iID;
			}

			bool operator!=(const EntityID& a_Other) const
			{
				return m_iID != a_Other.m_iID;
			}

			bool operator<(const EntityID& a_Other) const
			{
				return m_iID < a_Other.m_iID;
			}
		protected:
			enum ID_State : size_t
			{
				INVALID = 0
			};
			size_t m_iID = INVALID;
		};
	}
}