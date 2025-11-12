#include "ChunkFilter.h"

#include "./Defines.h"

namespace gallus
{
	namespace audio
	{
		ChunkFilter::ChunkFilter(const char* a_pData, size_t a_iSize)
		{
			m_pData = a_pData;
			m_iSize = a_iSize;
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
