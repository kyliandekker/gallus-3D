#pragma once

// external
#include <string>

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// EntityID
	//---------------------------------------------------------------------
	/// <summary>
	/// Lightweight identifier used to reference entities inside the ECS.
	/// Combines an index with a generation counter to prevent stale references.
	/// </summary>
	struct EntityID
	{
	public:
		/// <summary>
		/// Constructs an invalid entity identifier.
		/// </summary>
		EntityID() = default;

		/// <summary>
		/// Constructs a valid entity identifier with the specified index and generation.
		/// </summary>
		/// <param name="a_iIndex">Index of the entity in the internal storage.</param>
		/// <param name="a_iGeneration">Generation counter used to validate the entity.</param>
		EntityID(uint32_t a_iIndex, uint32_t a_iGeneration)
			: m_iIndex(a_iIndex)
			, m_iGeneration(a_iGeneration)
		{}

		/// <summary>
		/// Determines whether the entity identifier is valid.
		/// </summary>
		/// <returns>
		/// True if the identifier references a valid entity otherwise false.
		/// </returns>
		bool IsValid() const
		{
			return m_iIndex != INVALID_INDEX;
		}

		/// <summary>
		/// Retrieves the internal entity index.
		/// </summary>
		/// <returns>
		/// The index used to locate the entity in storage.
		/// </returns>
		uint32_t GetIndex() const
		{
			return m_iIndex;
		}

		/// <summary>
		/// Retrieves the generation counter.
		/// </summary>
		/// <returns>
		/// The generation value used to validate stale identifiers.
		/// </returns>
		uint32_t GetGeneration() const
		{
			return m_iGeneration;
		}

		/// <summary>
		/// Compares two entity identifiers for inequality.
		/// </summary>
		/// <param name="a_Other">The entity identifier to compare against.</param>
		/// <returns>
		/// True if the identifiers differ otherwise false.
		/// </returns>
		bool operator!=(const EntityID& a_Other) const
		{
			return !(*this == a_Other);
		}

		/// <summary>
		/// Provides strict ordering for entity identifiers.
		/// First compares indices then generations.
		/// </summary>
		/// <param name="a_Other">The entity identifier to compare against.</param>
		/// <returns>
		/// True if this identifier is considered smaller otherwise false.
		/// </returns>
		bool operator<(const EntityID& a_Other) const
		{
			if (m_iIndex != a_Other.m_iIndex)
			{
				return m_iIndex < a_Other.m_iIndex;
			}

			return m_iGeneration < a_Other.m_iGeneration;
		}

		/// <summary>
		/// Compares two entity identifiers for equality.
		/// </summary>
		/// <param name="a_Other">The entity identifier to compare against.</param>
		/// <returns>
		/// True if both index and generation match otherwise false.
		/// </returns>
		bool operator==(const EntityID& a_Other) const
		{
			return m_iIndex == a_Other.m_iIndex &&
				m_iGeneration == a_Other.m_iGeneration;
		}
	private:
		static constexpr uint32_t INVALID_INDEX = 0xFFFFFFFFu;

		uint32_t m_iIndex = INVALID_INDEX;
		uint32_t m_iGeneration = 0;
	};
}