#pragma once

// base class
#include "Data.h"

// external
#include <cstdint>

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// DataStream
		//---------------------------------------------------------------------
		/// <summary>
		/// Represents a generic container for raw data, providing utility functions and automatic cleanup
		/// for managing and interacting with the stored data. Allows the user to move the offset for reading and writing data at specific points.
		/// </summary>
		class DataStream : public Data
		{
		public:
			/// <summary>
			/// Constructs a data stream without any details.
			/// </summary>
			DataStream() = default;

			/// <summary>
			/// Constructs a data stream with the specified raw data and size.
			/// </summary>
			/// <param name="a_pData">Pointer to the raw data.</param>
			/// <param name="a_iSize">Size of the data in bytes.</param>
			DataStream(void* a_pData, size_t a_iSize);

			/// <summary>
			/// Constructs a data stream with the specified character data and size.
			/// </summary>
			/// <param name="a_pData">Pointer to the character data.</param>
			/// <param name="a_iSize">Size of the data in bytes.</param>
			DataStream(const char* a_pData, size_t a_iSize);

			/// <summary>
			/// Constructs a data stream with a specified size, allocating memory.
			/// </summary>
			/// <param name="a_iSize">Size of the data in bytes.</param>
			DataStream(size_t a_iSize);

			/// <summary>
			/// Copy constructor.
			/// </summary>
			/// <param name="a_Rhs">The DataStream object to copy from.</param>
			DataStream(const DataStream& a_Rhs);

			/// <summary>
			/// Copy constructor.
			/// </summary>
			/// <param name="a_Rhs">The DataStream object to copy from.</param>
			DataStream(const Data& a_Rhs);

			/// <summary>
			/// Copy assignment operator.
			/// </summary>
			/// <param name="a_Other">The DataStream object to copy from.</param>
			/// <returns>A reference to the current instance.</returns>
			DataStream& operator=(const DataStream& a_Other);

			/// <summary>
			/// Move constructor.
			/// </summary>
			/// <param name="a_Rhs">The Data object to copy from.</param>
			DataStream(DataStream&& a_Other) noexcept;

			/// <summary>
			/// Move assignment operator.
			/// </summary>
			/// <param name="a_Other">The Data object to copy from.</param>
			/// <returns>A reference to the current instance.</returns>
			DataStream& operator=(DataStream&& a_Other) noexcept;

			/// <summary>
			/// Frees the allocated memory for the data.
			/// </summary>
			void Free() override;

			/// <summary>
			/// Stores the data from the DataStream in a memory stream provided by the user.
			/// </summary>
			/// <param name="a_pData">The data to store other data into.</param>
			/// <param name="a_iDataSize">Size of the data in bytes.</param>
			/// <param name="a_iSize">No clue.</param>
			bool Read(void* a_pData, size_t a_iDataSize, size_t a_iSize);

			/// <summary>
			/// Writes data to the data stream.
			/// </summary>
			virtual bool Write(void const* a_pData, size_t a_iSize);

			/// <summary>
			/// Adjusts the current position within the data buffer based on the specified offset and origin.
			/// </summary>
			/// <param name="a_iOffset">The offset, in bytes, to move the current position.</param>
			/// <param name="a_iWhence">
			/// The reference point for the offset. Possible values:
			/// - SEEK_SET/0: Beginning of the data.
			/// - SEEK_CUR/1: Current position.
			/// - SEEK_END/2: End of the data.
			/// </param>
			/// <returns>True if the seek operation is successful, otherwise false.</returns>
			bool Seek(size_t a_iOffset, size_t a_iWhence);

			/// <summary>
			/// Retrieves the current position within the data buffer.
			/// </summary>
			/// <returns>The current position, in bytes, within the data buffer.</returns>
			size_t Tell() const;
		protected:
			size_t m_iPos = 0; /// The current position within the data buffer.
		};
	}
}