#include <cstdio>
#include <cstring>

#include "ChunkCollection.h"

// core includes
#include "core/Memory.h"

// logger includes
#include "logger/Logger.h"

// audio includes
#include "audio/Utils.h"

namespace gallus
{
	namespace audio
	{
		ChunkCollection::ChunkCollection(size_t a_iSize) : core::ReserveDataStream(a_iSize)
		{}

		ChunkCollection::ChunkCollection(const ReserveDataStream& a_Rhs) : core::ReserveDataStream(a_Rhs)
		{}

		ChunkCollection::ChunkCollection(const ChunkCollection& a_Rhs) : core::ReserveDataStream(a_Rhs)
		{}

		ChunkHeader* ChunkCollection::GetChunkFromAllocator(const char* a_ChunkID) const
		{
			size_t currentPos = 0;
			while (currentPos < m_iSize)
			{
				ChunkHeader* wave_chunk_data = reinterpret_cast<ChunkHeader*>(core::add(m_pData, currentPos));
				if (utils::chunkcmp(a_ChunkID, reinterpret_cast<const char*>(wave_chunk_data->m_aChunkId)) == 0)
				{
					return wave_chunk_data;
				}
				currentPos += wave_chunk_data->ChunkSize() + sizeof(ChunkHeader);
			}

			return nullptr;
		}
		
		ChunkHeader* ChunkCollection::GetChunkFromAllocator(uint32_t a_Index) const
		{
			size_t index = 0;
			size_t currentPos = 0;
			while (currentPos < m_iSize)
			{
				ChunkHeader* wave_chunk_data = reinterpret_cast<ChunkHeader*>(core::add(m_pData, currentPos));
				if (index == a_Index)
				{
					return wave_chunk_data;
				}
				index++;
				currentPos += wave_chunk_data->ChunkSize() + sizeof(ChunkHeader);
			}
			return nullptr;
		}
		
		WAVE_READER_RESULT ChunkCollection::GetChunkSize(uint32_t& a_Size, const char* a_ChunkID) const
		{
			a_Size = 0;
			ChunkHeader* data = GetChunkFromAllocator(a_ChunkID);
			if (data != nullptr)
			{
				a_Size = data->ChunkSize();
				return WAVE_READER_RESULT::OK;
			}

			return WAVE_READER_RESULT::ERR_CHUNK_NOT_FOUND;
		}
		
		WAVE_READER_RESULT ChunkCollection::GetNumberOfChunks(size_t& a_Size) const
		{
			a_Size = 0;
			size_t currentPos = 0;
			while (currentPos < m_iSize)
			{
				ChunkHeader* wave_chunk_data = reinterpret_cast<ChunkHeader*>(core::add(m_pData, currentPos));
				currentPos += wave_chunk_data->ChunkSize() + sizeof(ChunkHeader);
				a_Size++;
			}

			return WAVE_READER_RESULT::OK;
		}
		
		WAVE_READER_RESULT ChunkCollection::HasChunk(bool& a_ChunkFound, const char* a_ChunkID) const
		{
			a_ChunkFound = false;

			ChunkHeader* data = GetChunkFromAllocator(a_ChunkID);
			if (data != nullptr)
			{
				a_ChunkFound = true;
				return WAVE_READER_RESULT::OK;
			}

			return WAVE_READER_RESULT::ERR_CHUNK_NOT_FOUND;
		}
	}
}