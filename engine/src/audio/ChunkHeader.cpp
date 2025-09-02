#include "ChunkHeader.h"

#include <cstring>

#include "Utils.h"

namespace uaudio
{
	namespace wave_reader
	{
		ChunkHeader::ChunkHeader(const ChunkHeader& rhs)
		{
			memcpy(chunk_id, rhs.chunk_id, CHUNK_ID_SIZE);
			memcpy(chunkSize, rhs.chunkSize, sizeof(uint32_t));
		}

		ChunkHeader::ChunkHeader(ChunkHeader * a_DataBuffer)
		{
			memcpy(chunk_id, a_DataBuffer->chunk_id, CHUNK_ID_SIZE);
			memcpy(chunkSize, a_DataBuffer->chunkSize, sizeof(uint32_t));
		}

		uint32_t ChunkHeader::ChunkSize(bool isBigEndian) const
		{
			uint32_t size32 = 0;
			memcpy(&size32, &chunkSize, sizeof(uint32_t));
			if (isBigEndian)
				size32 = utils::reverseBytesC<uint32_t>(reinterpret_cast<unsigned char*>(&size32));
			return size32;
		}

		void ChunkHeader::SetChunkSize(uint32_t chunk_size, bool toBigEndian)
		{
			uint32_t size32 = chunk_size;
			memcpy(this->chunkSize, reinterpret_cast<unsigned char*>(&size32), sizeof(uint32_t));
			if (toBigEndian)
				utils::reverseBytes(this->chunkSize, sizeof(uint32_t));
		}
	}
}