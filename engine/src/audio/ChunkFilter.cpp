#include "ChunkFilter.h"

#include "./Defines.h"

namespace uaudio
{
	namespace wave_reader
	{
		ChunkFilter::ChunkFilter(const char* a_Filter, size_t a_Size)
        {
			m_Data = a_Filter;
			m_Size = a_Size;
        }

		const char* ChunkFilter::data() const
		{
			return m_Data;
		}

		size_t ChunkFilter::size() const
		{
			return m_Size;
		}

		const char* ChunkFilter::operator[](size_t i) const
		{
			return m_Data + (i * CHUNK_ID_SIZE);
		}
	}
}