#pragma once

// external
#include <cstdint>

// audio
#include "audio/Defines.h"

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
			ChunkHeader() {};
			ChunkHeader(const ChunkHeader& rhs);
			ChunkHeader(ChunkHeader* a_DataBuffer);

			char m_aChunkId[audio::CHUNK_ID_SIZE] = {};
			char m_aChunkSize[sizeof(uint32_t)] = {};

			uint32_t ChunkSize(bool isBigEndian = false) const;
			void SetChunkSize(uint32_t chunk_size, bool toBigEndian = false);
		};

		using WaveHeader = ChunkHeader;
		using Header = ChunkHeader;
#pragma pack(pop)
	}
}
