#include "core/ReserveDataStream.h"

// external
#include <corecrt_malloc.h>

// core
#include "core/Memory.h"

namespace gallus::core
{
	//---------------------------------------------------------------------
	// ReserveDataStream
	//---------------------------------------------------------------------
	ReserveDataStream::ReserveDataStream(size_t a_iSize) : DataStream(a_iSize), m_iReservedSize(a_iSize)
	{}

	//---------------------------------------------------------------------
	ReserveDataStream::ReserveDataStream(const ReserveDataStream& rhs) : DataStream(rhs)
	{
		m_iReservedSize = rhs.m_iReservedSize;
	}

	//---------------------------------------------------------------------
	ReserveDataStream::ReserveDataStream(const DataStream& rhs) : DataStream(rhs)
	{
		m_iReservedSize = rhs.size();
	}

	//---------------------------------------------------------------------
	ReserveDataStream& ReserveDataStream::operator=(const ReserveDataStream& a_Other)
	{
		if (&a_Other != this)
		{
			DataStream::operator=(a_Other);
			m_iReservedSize = a_Other.m_iReservedSize;
		}
		return *this;
	}

	//---------------------------------------------------------------------
	void ReserveDataStream::Free()
	{
		DataStream::Free();
		m_iReservedSize = 0;
	}

	//---------------------------------------------------------------------
	bool ReserveDataStream::Write(void const* a_pData, size_t a_iSize)
	{
		const size_t size = m_iSize;
		if (m_iPos == m_iReservedSize || m_iPos + a_iSize > m_iReservedSize)
		{
			Reallocate(a_iSize);
		}
		if (m_iPos == size || m_iPos + a_iSize > size)
		{
			m_iSize += a_iSize;
		}

		memcpy(add(m_pData, m_iPos), a_pData, a_iSize);
		Seek(a_iSize, SEEK_CUR);
		return true;
	}

	//---------------------------------------------------------------------
	bool ReserveDataStream::Seek(size_t a_iOffset, size_t a_iWhence)
	{
		switch (a_iWhence)
		{
			case SEEK_SET:
			{
				m_iPos = a_iOffset;
				return true;
			}
			case SEEK_CUR:
			{
				m_iPos += a_iOffset;
				return true;
			}
			case SEEK_END:
			default:
			{
				m_iPos = m_iSize - a_iOffset;
				return true;
			}
		}
		return false;
	}

	//---------------------------------------------------------------------
	void ReserveDataStream::Reallocate(size_t a_iExtraSize)
	{
		if (a_iExtraSize == 0)
		{
			return;
		}
		const size_t newReservedSize = m_iSize + (a_iExtraSize * 2000);
		void* newData = malloc(newReservedSize);
		if (!newData)
		{
			return;
		}
		memcpy(newData, m_pData, m_iSize);

		if (m_pData)
		{
			free(m_pData);
		}
		m_iReservedSize = newReservedSize;
		m_pData = newData;
	}

	//---------------------------------------------------------------------
	size_t ReserveDataStream::Tell() const
	{
		return m_iPos;
	}
}