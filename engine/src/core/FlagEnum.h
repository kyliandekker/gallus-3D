#pragma once

// external
#include <type_traits>

namespace gallus
{
	namespace core
	{
		template<typename Enum>
		class FlagEnum
		{
		public:
			using T = std::underlying_type_t<Enum>;

			FlagEnum()
				: m_iFlags(0)
			{
			}

			FlagEnum(Enum a_eFlag)
				: m_iFlags(static_cast<T>(a_eFlag))
			{
			}

			void AddFlag(Enum a_eFlag)
			{
				m_iFlags |= static_cast<T>(a_eFlag);
			}

			void RemoveFlag(Enum a_eFlag)
			{
				m_iFlags &= ~static_cast<T>(a_eFlag);
			}

			bool HasFlag(Enum a_eFlag) const
			{
				return (m_iFlags & static_cast<T>(a_eFlag)) != 0;
			}
			
			bool HasFlagOnly(Enum a_eFlag) const
			{
				return m_iFlags == static_cast<T>(a_eFlag);
			}

			void Clear()
			{
				m_iFlags = 0;
			}

			T GetValue() const
			{
				return m_iFlags;
			}

			void SetValue(T a_iFlags)
			{
				m_iFlags = a_iFlags;
			}

		private:
			T m_iFlags;
		};
	}
}
