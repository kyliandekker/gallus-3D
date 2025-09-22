#include "WaveChunks.h"

#include <cstring>

#include "core/Memory.h"

namespace gallus
{
	namespace audio
	{
		RIFF_Chunk::RIFF_Chunk(RIFF_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				memcpy(m_sFormat, a_DataBuffer->m_sFormat, sizeof(m_sFormat));
			}
		}

		FMT_Chunk::FMT_Chunk(FMT_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				m_iAudioFormat = a_DataBuffer->m_iAudioFormat;
				m_iNumChannels = a_DataBuffer->m_iNumChannels;
				m_iSampleRate = a_DataBuffer->m_iSampleRate;
				m_iByteRate = a_DataBuffer->m_iByteRate;
				m_iBlockAlign = a_DataBuffer->m_iBlockAlign;
				m_iBitsPerSample = a_DataBuffer->m_iBitsPerSample;
			}
		}

		DATA_Chunk::DATA_Chunk(DATA_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				// data is a pointer to everything after the data header.
				m_pData = reinterpret_cast<unsigned char*>(core::add(a_DataBuffer, sizeof(DATA_Chunk) - sizeof(m_pData)));
			}
		}
	}
}