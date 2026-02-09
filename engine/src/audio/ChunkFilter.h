#pragma once

// external
#include <cstdint>

namespace gallus
{
	namespace audio
	{
		/*
		 * This is a filter for the wave file loader. If you want to only get specific chunks, use this.
		*/
		class ChunkFilter
		{
		public:
			/// <summary>
			/// Constructs Chunk Filter.
			/// </summary>
			ChunkFilter() = default;

			/// <summary>
			/// Constructs Chunk Filter with chunks filled.
			/// </summary>
			/// <param name="a_pData">Data containing the chunks.</param>
			/// <param name="a_iSize">Size of the data in bytes.</param>
			ChunkFilter(const char* a_pData, size_t a_iSize);

			/// <summary>
			/// Retrieves the chunk data.
			/// </summary>
			/// <returns>Pointer to the data containing chunk names.</returns>
			const char* data() const;

			/// <summary>
			/// Retrieves the chunk data size.
			/// </summary>
			/// <returns>Size of the data pointer.</returns>
			size_t size() const;

			/// <summary>
			/// Retrieves a specific chunk in the data array.
			/// </summary>
			/// <returns>The chunk based on the index.</returns>
			const char* operator [](size_t i) const;
		private:
			size_t m_iSize = 0;
			const char* m_pData = nullptr;
		};
	}
}