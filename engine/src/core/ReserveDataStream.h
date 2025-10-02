#pragma once

// base class
#include "DataStream.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// ReserveDataStream
		//---------------------------------------------------------------------
		/// <summary>
		/// Represents a generic container for raw data, providing utility functions and automatic cleanup
		/// for managing and interacting with the stored data. Allows the user to move the offset for reading and writing data at specific points.
		/// Also reallocates when out of memory.
		/// </summary>
		class ReserveDataStream : public DataStream
		{
		public:
			ReserveDataStream() = default;

			/// <summary>
			/// Constructs a Data object with the specified size.
			/// </summary>
			/// <param name="a_iSize">Size of the data in bytes.</param>
			ReserveDataStream(size_t a_iSize);

			/// <summary>
			/// Copy constructor.
			/// </summary>
			/// <param name="a_Rhs">The Data object to copy from.</param>
			ReserveDataStream(const ReserveDataStream& a_Rhs);

			/// <summary>
			/// Copy constructor.
			/// </summary>
			/// <param name="a_Rhs">The Data object to copy from.</param>
			ReserveDataStream(const DataStream& a_Rhs);

			/// <summary>
			/// Copy assignment operator.
			/// </summary>
			/// <param name="a_Other">The Data object to copy from.</param>
			/// <returns>A reference to the current instance.</returns>
			ReserveDataStream& operator=(const ReserveDataStream& a_Other);

			/// <summary>
			/// Frees the allocated memory for the data.
			/// </summary>
			void Free() override;

			/// <summary>
			/// Writes data to the data stream.
			/// </summary>
			/// <param name="a_pData">The data to write.</param>
			/// <param name="a_iSize">Size of the data in bytes.</param>
			bool Write(void const* a_pData, size_t a_iSize) override;

			/// <summary>
			/// Moves the read/write position in the stream.
			/// </summary>
			/// <param name="a_iOffset">The offset to seek to.</param>
			/// <param name="a_iWhence">The starting point for the offset (e.g., beginning, current, or end).</param>
			/// <returns>True if the seek operation was successful, false otherwise.</returns>
			bool Seek(size_t a_iOffset, size_t a_iWhence);

			/// <summary>
			/// Retrieves the current position in the stream.
			/// </summary>
			/// <returns>The current offset in the stream.</returns>
			size_t Tell() const;
		protected:
			/// <summary>
			/// Reallocates the allocated memory to a new size and copies the data.
			/// </summary>
			/// <param name="a_iExtraSize">The size to add to the current allocated memory.</param>
			void Reallocate(size_t a_iExtraSize);

			size_t m_iReservedSize = 0;
		};
	}
}