#include "WaveReadSettings.h"

#include "UAUDIO_WAVE_READER_RESULT.h"

namespace uaudio
{
	namespace wave_reader
	{
		WaveReadSettings::WaveReadSettings()
		{ }

		WaveReadSettings::WaveReadSettings(ChunkFilter& a_ChunkFilter) : m_ChunkFilter(&a_ChunkFilter)
		{ }

		UAUDIO_WAVE_READER_RESULT WaveReadSettings::Channels(ChannelsConversionSettings& a_Channels) const
		{
			a_Channels = m_Channels;
			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}

		UAUDIO_WAVE_READER_RESULT WaveReadSettings::SetChunkFilter(ChunkFilter& a_ChunkFilter)
		{
			m_ChunkFilter = &a_ChunkFilter;
			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}

		UAUDIO_WAVE_READER_RESULT WaveReadSettings::BitsPerSample(BPSConversionSettings& a_BitsPerSample) const
		{
			a_BitsPerSample = m_BitsPerSample;
			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}

		UAUDIO_WAVE_READER_RESULT WaveReadSettings::SetChannels(ChannelsConversionSettings a_Channels)
		{
			m_Channels = a_Channels;
			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}

		UAUDIO_WAVE_READER_RESULT WaveReadSettings::SetBitsPerSample(BPSConversionSettings a_BitsPerSample)
		{
			m_BitsPerSample = a_BitsPerSample;
			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}
	}
}