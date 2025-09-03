#include "AudioUtils.h"

// core includes
#include "core/DataStream.h"

#include "audio/Defines.h"
#include "audio/int24_t.h"

namespace gallus
{
	namespace audio
	{
		bool ToSample(core::Data& a_Data, unsigned char* a_pAudioData, size_t a_iBuffersize, uint16_t a_iBitsPerSample, uint16_t a_iBlockAlign, uint16_t a_iAudioFormat, size_t a_iNumSamples, bool a_bLeft)
		{
			if (a_pAudioData == nullptr)
			{
				return false;
			}

			unsigned char* pData = a_pAudioData;
			size_t realNumSamples = a_iBuffersize / a_iBlockAlign;

			if (realNumSamples == 0)
			{
				return false;
			}

			size_t div = realNumSamples / a_iNumSamples;

			core::DataStream data = core::DataStream(a_iNumSamples * sizeof(double));

			if (!a_bLeft)
			{
				pData += a_iBlockAlign / 2;
			}
			for (size_t i = 0; i < a_iNumSamples; i++)
			{
				double val = 0.0;
				if (a_iBitsPerSample == WAVE_BITS_PER_SAMPLE_8)
				{
					int8_t psample = INT8_MAX - *(uint8_t*) pData;
					val = static_cast<double>(psample) / INT8_MAX;
				}
				else if (a_iBitsPerSample == WAVE_BITS_PER_SAMPLE_16)
				{
					int16_t sample = *(int16_t*) pData;
					val = static_cast<double>(sample) / INT16_MAX;
				}
				else if (a_iBitsPerSample == WAVE_BITS_PER_SAMPLE_24)
				{
					int24_t sample = *(int24_t*) pData;
					val = static_cast<double>(sample) / INT24_MAX;
				}
				else if (a_iBitsPerSample == WAVE_BITS_PER_SAMPLE_32_FLOAT && a_iAudioFormat == WAV_FORMAT_IEEE_FLOAT)
				{
					double sample = *(float*) pData;
					val = static_cast<double>(sample);
				}
				else if (a_iBitsPerSample == WAVE_BITS_PER_SAMPLE_32)
				{
					double sample = *(int32_t*) pData;
					val = static_cast<double>(sample) / INT32_MAX;
				}
				else if (a_iBitsPerSample == WAVE_BITS_PER_SAMPLE_64)
				{
					double sample = *(double*) pData;
					val = static_cast<double>(sample);
				}

				data.Write(reinterpret_cast<const void*>(&val), sizeof(val));
				pData += div * a_iBlockAlign;
			}

			a_Data = data;
			return true;
		}
	}
}