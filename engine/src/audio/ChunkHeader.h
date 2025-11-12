#pragma once

#include <cstdint>

#include "./Defines.h"

namespace gallus
{
	namespace audio
	{
#pragma pack(push, 1)
		/*
		 * WHAT IS THIS FILE?
		 * This is a wave chunk header. Every chunk in a wave file has an ID and size.
		 * These are used in the WaveFormat class, where it holds every loaded chunk
		 * in an array.
		 *
			* The chunk id is always 4 bytes long and needs to be unique. Capitalization matters.
			* The chunk size is different for every chunk and determines how many bytes are read after this chunk header.
		 */
		struct ChunkHeader
		{
			/// <summary>
			/// Constructs chunk header.
			/// </summary>
			ChunkHeader() = default;

			/// <summary>
			/// Constructs chunk header based on a copy.
			/// </summary>
			/// <param name="rhs">The copy.</param>
			ChunkHeader(const ChunkHeader& rhs);

			/// <summary>
			/// Constructs chunk header based on a data buffer.
			/// </summary>
			/// <param name="a_DataBuffer">The data buffer.</param>
			ChunkHeader(ChunkHeader* a_DataBuffer);

			char m_aChunkId[audio::CHUNK_ID_SIZE] = {};
			char m_aChunkSize[sizeof(uint32_t)] = {};

			/// <summary>
			/// The size of the data.
			/// </summary>
			/// <param name="a_bIsBigEndian">Whether to reverse the data or not.</param>
			/// <returns>The size of the data.</returns>
			uint32_t ChunkSize(bool a_bIsBigEndian = false) const;

			/// <summary>
			/// Sets the size of the data.
			/// </summary>
			/// <param name="a_iChunkSize">The new size of the data.</param>
			/// <param name="a_bIsBigEndian">Whether to reverse the data or not.</param>
			/// <returns>The size of the data.</returns>
			void SetChunkSize(uint32_t a_iChunkSize, bool a_bIsBigEndian = false);
		};

		using WaveHeader = ChunkHeader;
		using Header = ChunkHeader;
#pragma pack(pop)
	}
}
