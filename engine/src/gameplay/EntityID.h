#pragma once

// external
#include <string>

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// EntityID
		//---------------------------------------------------------------------
		struct EntityID
		{
		public:
			EntityID() = default;

			EntityID(uint32_t a_iIndex, uint32_t a_iGeneration)
				: m_iIndex(a_iIndex)
				, m_iGeneration(a_iGeneration)
			{}

			bool IsValid() const
			{
				return m_iIndex != INVALID_INDEX;
			}

			uint32_t GetIndex() const
			{
				return m_iIndex;
			}

			uint32_t GetGeneration() const
			{
				return m_iGeneration;
			}

			bool operator!=(const EntityID& a_Other) const
			{
				return !(*this == a_Other);
			}

			bool operator<(const EntityID& a_Other) const
			{
				if (m_iIndex != a_Other.m_iIndex)
				{
					return m_iIndex < a_Other.m_iIndex;
				}

				return m_iGeneration < a_Other.m_iGeneration;
			}

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
}