#pragma once

// external
#include <type_traits>

namespace gallus::core
{
	/// <summary>
	/// Utility wrapper for strongly-typed enum classes used as bit flags.
	/// Provides bitwise flag manipulation while preserving type safety.
	/// </summary>
	/// <typeparam name="Enum">
	/// Enum type representing the flags. Must be an enum type.
	/// </typeparam>
	template<typename Enum>
	class FlagEnum
	{
	public:
		using T = std::underlying_type_t<Enum>;

		/// <summary>
		/// Constructs an empty flag container with no flags set.
		/// </summary>
		FlagEnum()
			: m_iFlags(0)
		{
		}

		/// <summary>
		/// Constructs the flag container initialized with a single flag.
		/// </summary>
		/// <param name="a_eFlag">The initial flag to set.</param>
		FlagEnum(Enum a_eFlag)
			: m_iFlags(static_cast<T>(a_eFlag))
		{
		}

		/// <summary>
		/// Adds the specified flag to the container.
		/// </summary>
		/// <param name="a_eFlag">The flag to add.</param>
		void AddFlag(Enum a_eFlag)
		{
			m_iFlags |= static_cast<T>(a_eFlag);
		}

		/// <summary>
		/// Removes the specified flag from the container.
		/// </summary>
		/// <param name="a_eFlag">The flag to remove.</param>
		void RemoveFlag(Enum a_eFlag)
		{
			m_iFlags &= ~static_cast<T>(a_eFlag);
		}

		/// <summary>
		/// Determines whether the specified flag is set.
		/// </summary>
		/// <param name="a_eFlag">The flag to check.</param>
		/// <returns>
		/// True if the flag is set, otherwise false.
		/// </returns>
		bool HasFlag(Enum a_eFlag) const
		{
			return (m_iFlags & static_cast<T>(a_eFlag)) != 0;
		}

		/// <summary>
		/// Determines whether the container contains only the specified flag.
		/// </summary>
		/// <param name="a_eFlag">The flag to compare against.</param>
		/// <returns>
		/// True if the container contains exactly the specified flag and no others, otherwise false.
		/// </returns>
		bool HasFlagOnly(Enum a_eFlag) const
		{
			return m_iFlags == static_cast<T>(a_eFlag);
		}

		/// <summary>
		/// Clears all flags from the container.
		/// </summary>
		void Clear()
		{
			m_iFlags = 0;
		}

		/// <summary>
		/// Retrieves the raw underlying flag value.
		/// </summary>
		/// <returns>
		/// The underlying integral representation of the flags.
		/// </returns>
		T GetValue() const
		{
			return m_iFlags;
		}

		/// <summary>
		/// Sets the raw underlying flag value.
		/// </summary>
		/// <param name="a_iFlags">
		/// The underlying integral value representing the flags.
		/// </param>
		void SetValue(T a_iFlags)
		{
			m_iFlags = a_iFlags;
		}
	private:
		T m_iFlags;
	};
}