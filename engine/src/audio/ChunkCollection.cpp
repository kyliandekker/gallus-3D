#include <cstdio>
#include <cstring>

#include "ChunkCollection.h"
#include "Logger.h"

namespace uaudio
{
	namespace wave_reader
	{
		ChunkCollection::ChunkCollection(void* a_Ptr, size_t a_Size)
		{
			if (a_Size > 0)
			{
				m_Size = a_Size;
				m_Start = a_Ptr;
				m_Allocated = 0;
				m_End = utils::add(m_Start, m_Size);
			}
			else
			{
				logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Cannot allocate with size of 0.");
			}
		}

		ChunkHeader* ChunkCollection::GetChunkFromAllocator(const char* a_ChunkID) const
		{
			if (m_Start == nullptr)
			{
				logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Chunk collection has not been initialized.");
				return nullptr;
			}

			unsigned char* data = reinterpret_cast<unsigned char*>(m_Start);
			while (data < m_End)
			{
				ChunkHeader* wave_chunk_data = reinterpret_cast<ChunkHeader*>(data);
				if (strncmp(&a_ChunkID[0], &reinterpret_cast<char*>(wave_chunk_data->chunk_id)[0], CHUNK_ID_SIZE) == 0)
				{
					return wave_chunk_data;
				}
				data = reinterpret_cast<unsigned char*>(utils::add(data, sizeof(ChunkHeader) + wave_chunk_data->ChunkSize()));
			}
			return nullptr;
		}
		
		ChunkHeader* ChunkCollection::GetChunkFromAllocator(uint32_t a_Index) const
		{
			if (m_Start == nullptr)
			{
				logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Chunk collection has not been initialized.");
				return nullptr;
			}

			unsigned char* data = reinterpret_cast<unsigned char*>(m_Start);
			uint32_t index = 0;
			while (data < m_End)
			{
				ChunkHeader* wave_chunk_data = reinterpret_cast<ChunkHeader*>(data);
				data = reinterpret_cast<unsigned char*>(utils::add(data, sizeof(ChunkHeader) + wave_chunk_data->ChunkSize()));
				if (index == a_Index)
					return wave_chunk_data;
				index++;
			}
			return nullptr;
		}
		
		void* ChunkCollection::Alloc(size_t a_Size)
		{
			if (m_Start == nullptr)
			{
				logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Chunk collection has not been initialized.");
				return nullptr;
			}
			if (m_Size == 0)
			{
				logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Chunk collection has a size of 0.");
				return nullptr;
			}
			if (m_Allocated + a_Size > m_Size)
			{
				logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Chunk collection does not have enough memory allocated.");
				return nullptr;
			}

			void* current = utils::add(m_Start, m_Allocated);
			m_Allocated += a_Size;
			memset(current, 0, a_Size);
			return current;
		}

		void* ChunkCollection::Alloc(size_t a_Size, const char* a_ChunkId)
		{
			ChunkHeader* chunkCollection = reinterpret_cast<ChunkHeader*>(Alloc(a_Size));
			if (chunkCollection)
			{
				memcpy(chunkCollection->chunk_id, a_ChunkId, CHUNK_ID_SIZE);
				chunkCollection->SetChunkSize(static_cast<uint32_t>(a_Size));
			}
			return reinterpret_cast<void*>(chunkCollection);
		}
		
		void* ChunkCollection::End() const
		{
			return m_End;
		}

        void ChunkCollection::Realloc(void* a_Buffer, size_t a_Size)
        {
			if (m_Start == nullptr)
			{
				logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Chunk collection has not been initialized.");
				return;
			}
			if (a_Size < m_Size)
			{
				logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Cannot reallocate memory of less size than previous memory.");
				return;
			}

			memmove(a_Buffer, m_Start, a_Size);
			m_Start = a_Buffer;
			m_Size = a_Size;
			m_End = utils::add(m_Start, m_Size);
        }
		
		void* ChunkCollection::data() const
		{
			return m_Start;
		}
		
		UAUDIO_WAVE_READER_RESULT ChunkCollection::GetChunkSize(uint32_t& a_Size, const char* a_ChunkID) const
		{
			a_Size = 0;
			ChunkHeader* data = GetChunkFromAllocator(a_ChunkID);
			if (data != nullptr)
			{
				a_Size = data->ChunkSize();
				return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
			}

			return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
		}
		
		UAUDIO_WAVE_READER_RESULT ChunkCollection::GetNumberOfChunks(size_t& a_Size) const
		{
			if (m_Start == nullptr)
			{
				logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Chunk collection has not been initialized.");
				return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_CHUNK_COLLECTION_NOT_INITIALIZED;
			}

			unsigned char* data = reinterpret_cast<unsigned char*>(m_Start);
			a_Size = 0;
			while (data < m_End)
			{
				ChunkHeader* wave_chunk_data = reinterpret_cast<ChunkHeader*>(data);
				data = reinterpret_cast<unsigned char*>(utils::add(data, sizeof(ChunkHeader) + wave_chunk_data->ChunkSize()));
				a_Size++;
			}
			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}
		
		UAUDIO_WAVE_READER_RESULT ChunkCollection::HasChunk(bool& a_ChunkFound, const char* a_ChunkID) const
		{
			a_ChunkFound = false;

			ChunkHeader* data = GetChunkFromAllocator(a_ChunkID);
			if (data != nullptr)
			{
				a_ChunkFound = true;
				return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
			}

			return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
		}

		size_t ChunkCollection::size() const
		{
			return m_Size;
		}
	}
}