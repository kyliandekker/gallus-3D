#include "ChunkFilter.h"

#include "./Defines.h"

namespace gallus
{
	namespace audio
	{
		ChunkFilter::ChunkFilter(const char* a_Filter, size_t a_Size)
        {
			m_pData = a_Filter;
			m_iSize = a_Size;
        }

		const char* ChunkFilter::data() const
		{
			return m_pData;
		}

		size_t ChunkFilter::size() const
		{
			return m_iSize;
		}

		const char* ChunkFilter::operator[](size_t i) const
		{
			return m_pData + (i * CHUNK_ID_SIZE);
		}
	}
}