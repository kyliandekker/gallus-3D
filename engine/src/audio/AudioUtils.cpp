#include "AudioUtils.h"

// core
#include "core/DataStream.h"

// audio
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

		float PosToMilliseconds(uint32_t m_Pos, uint32_t a_ByteRate)
		{
			return SecondsToMilliseconds(PosToSeconds(m_Pos, a_ByteRate));
		}

		float PosToSeconds(uint32_t m_Pos, uint32_t a_ByteRate)
		{
			if (a_ByteRate == 0)
				return 0;

			return static_cast<float>(m_Pos) / static_cast<float>(a_ByteRate);
		}

		uint32_t SecondsToPos(float m_Seconds, uint32_t a_ByteRate)
		{
			if (a_ByteRate == 0)
				return 0;

			return static_cast<uint32_t>(m_Seconds * static_cast<float>(a_ByteRate));
		}

		uint32_t MillisecondsToPos(float m_Milliseconds, uint32_t a_ByteRate)
		{
			if (a_ByteRate == 0)
				return 0;

			return SecondsToPos(MillisecondsToSeconds(m_Milliseconds), a_ByteRate);
		}

		float SecondsToMilliseconds(float m_Seconds)
		{
			return m_Seconds * 1000;
		}

		float MillisecondsToSeconds(float m_Milliseconds)
		{
			return m_Milliseconds / 1000;
		}

		uint32_t SecondsToHours(float m_Seconds)
		{
			return static_cast<uint32_t>(m_Seconds) / 3600;
		}

		uint32_t SecondsToMinutes(float m_Seconds)
		{
			return (static_cast<uint32_t>(m_Seconds) - (SecondsToHours(m_Seconds) * 3600)) / 60;
		}

		float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate)
		{
			return static_cast<float>(a_ChunkSize) / static_cast<float>(a_ByteRate);
		}

		std::string FormatDuration(float a_Duration, bool a_Milliseconds)
		{
			const uint32_t hours = SecondsToHours(a_Duration);
			const uint32_t minutes = SecondsToMinutes(a_Duration);
			const uint32_t seconds = static_cast<uint32_t>(a_Duration) % 60;
			const uint32_t total = (hours * 3600) + (minutes * 60) + seconds;
			const float milliseconds_float = a_Duration - static_cast<float>(total);
			const uint32_t milliseconds = static_cast<uint32_t>(milliseconds_float * 1000);

			char hours_string[32], minutes_string[32], seconds_string[32], milliseconds_string[32];
			sprintf_s(hours_string, "%02d", hours);
			sprintf_s(minutes_string, "%02d", minutes);
			sprintf_s(seconds_string, "%02d", seconds);
			sprintf_s(milliseconds_string, "%03d", milliseconds);
			return std::string(hours_string) +
				":" +
				std::string(minutes_string) +
				":" +
				std::string(seconds_string) + (a_Milliseconds ? ":" + std::string(milliseconds_string) : "");
		}
	}
}