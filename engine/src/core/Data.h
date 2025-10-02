#pragma once

// external
#include <cstdint>
#include <string>

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// Data
		//---------------------------------------------------------------------
		/// <summary>
		/// Represents a generic container for raw data, providing utility functions and automatic cleanup
		/// for managing and interacting with the stored data.
		/// </summary>
		class Data
		{
		public:
			Data() = default;

			/// <summary>
			/// Constructs a data object with the specified raw data and size.
			/// </summary>
			/// <param name="a_pData">Pointer to the raw data.</param>
			/// <param name="a_iSize">Size of the data in bytes.</param>
			Data(const void* a_pData, size_t a_iSize);

			/// <summary>
			/// Constructs a data object with the specified character data and size.
			/// </summary>
			/// <param name="a_pData">Pointer to the character data.</param>
			/// <param name="a_iSize">Size of the data in bytes.</param>
			Data(const char* a_pData, size_t a_iSize);

			/// <summary>
			/// Constructs a data object with a specified size, allocating memory.
			/// </summary>
			/// <param name="a_iSize">Size of the data in bytes.</param>
			Data(size_t a_iSize);

			/// <summary>
			/// Copy constructor.
			/// </summary>
			/// <param name="a_Rhs">The data object to copy from.</param>
			Data(const Data& a_Rhs);

			/// <summary>
			/// Copy assignment operator.
			/// </summary>
			/// <param name="a_Other">The data object to copy from.</param>
			/// <returns>A reference to the current instance.</returns>
			Data& operator=(const Data& a_Other);

			/// <summary>
			/// Move constructor.
			/// </summary>
			/// <param name="a_Rhs">The data object to copy from.</param>
			Data(Data&& a_Other) noexcept;

			/// <summary>
			/// Move assignment operator.
			/// </summary>
			/// <param name="a_Other">The data object to copy from.</param>
			/// <returns>A reference to the current instance.</returns>
			Data& operator=(Data&& a_Other) noexcept;

			/// <summary>
			/// Destructor to free allocated memory.
			/// </summary>
			virtual ~Data();

			/// <summary>
			/// Retrieves the size of the stored data in bytes.
			/// </summary>
			/// <returns>The size of the data.</returns>
			size_t size() const
			{
				return m_iSize;
			}

			/// <summary>
			/// Checks whether the data object is empty (contains no data).
			/// </summary>
			/// <returns>True if empty, otherwise false.</returns>
			bool empty() const
			{
				return m_pData == nullptr || m_iSize == 0;
			}

			/// <summary>
			/// Provides a raw pointer to the stored data.
			/// </summary>
			/// <returns>A void pointer to the data.</returns>
			const void* data() const
			{
				return m_pData;
			}

			/// <summary>
			/// Provides a raw pointer to the stored data.
			/// </summary>
			/// <returns>A void pointer to the data.</returns>
			void* data()
			{
				return m_pData;
			}

			/// <summary>
			/// Provides a typed pointer to the stored data.
			/// </summary>
			/// <typeparam name="T">The desired type of the pointer.</typeparam>
			/// <returns>A pointer to the data cast to type T.</returns>
			template <typename T>
			const T* dataAs() const
			{
				return reinterpret_cast<const T*>(m_pData);
			}

			/// <summary>
			/// Provides a typed pointer to the stored data.
			/// </summary>
			/// <typeparam name="T">The desired type of the pointer.</typeparam>
			/// <returns>A pointer to the data cast to type T.</returns>
			template <typename T>
			T* dataAs()
			{
				return reinterpret_cast<T*>(m_pData);
			}

			/// <summary>
			/// Frees the allocated memory for the data.
			/// </summary>
			virtual void Free();

			/// <summary>
			/// Clears the data but keeps the size the same.
			/// </summary>
			virtual void Clear();

			/// <summary>
			/// Accesses the byte at the specified index (read-only).
			/// </summary>
			/// <param name="a_iIndex">The index of the byte to access.</param>
			/// <returns>The byte at the specified index.</returns>
			unsigned char operator [] (size_t a_iIndex) const;

			/// <summary>
			/// Accesses the byte at the specified index (modifiable).
			/// </summary>
			/// <param name="a_iIndex">The index of the byte to access.</param>
			/// <returns>A reference to the byte at the specified index.</returns>
			unsigned char& operator [] (size_t a_iIndex);
		protected:
			void* m_pData = nullptr; /// Pointer to the raw data.
			size_t m_iSize = 0;      /// Size of the data in bytes.
		};
	}
}