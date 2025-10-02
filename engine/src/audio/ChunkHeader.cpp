// header
#include "ChunkHeader.h"

// external
#include <cstring>

// audio
#include "audio/Utils.h"

namespace gallus
{
	namespace audio
	{
		ChunkHeader::ChunkHeader(const ChunkHeader& rhs)
		{
			memcpy(m_aChunkId, rhs.m_aChunkId, CHUNK_ID_SIZE);
			memcpy(m_aChunkSize, rhs.m_aChunkSize, sizeof(uint32_t));
		}

		ChunkHeader::ChunkHeader(ChunkHeader* a_DataBuffer)
		{
			memcpy(m_aChunkId, a_DataBuffer->m_aChunkId, CHUNK_ID_SIZE);
			memcpy(m_aChunkSize, a_DataBuffer->m_aChunkSize, sizeof(uint32_t));
		}

		uint32_t ChunkHeader::ChunkSize(bool isBigEndian) const
		{
			uint32_t size32 = 0;
			memcpy(&size32, &m_aChunkSize, sizeof(uint32_t));
			if (isBigEndian)
			{
				size32 = utils::reverseBytesC<uint32_t>(reinterpret_cast<unsigned char*>(&size32));
			}
			return size32;
		}

		void ChunkHeader::SetChunkSize(uint32_t chunk_size, bool toBigEndian)
		{
			uint32_t size32 = chunk_size;
			memcpy(this->m_aChunkSize, reinterpret_cast<char*>(&size32), sizeof(uint32_t));
			if (toBigEndian)
			{
				utils::reverseBytes(reinterpret_cast<unsigned char*>(&m_aChunkSize), sizeof(uint32_t));
			}
		}
	}
}