#pragma once

#include <cstdint>
#include <cstdio>

#include "UAUDIO_WAVE_READER_RESULT.h"

namespace uaudio
{
	namespace wave_reader
	{
		namespace conversion
		{
			uint32_t Calculate8To16Size(uint32_t a_Size);
			uint32_t Calculate8To24Size(uint32_t a_Size);
			uint32_t Calculate8To32Size(uint32_t a_Size);

			uint32_t Calculate16To8Size(uint32_t a_Size);
			uint32_t Calculate16To32Size(uint32_t a_Size);
			uint32_t Calculate16To24Size(uint32_t a_Size);

			uint32_t Calculate24To8Size(uint32_t a_Size);
			uint32_t Calculate24To16Size(uint32_t a_Size);
			uint32_t Calculate24To32Size(uint32_t a_Size);

			uint32_t Calculate32To8Size(uint32_t a_Size);
			uint32_t Calculate32To16Size(uint32_t a_Size);
			uint32_t Calculate32To24Size(uint32_t a_Size);

			uint32_t CalculateMonoToStereoSize(uint32_t a_Size);
			uint32_t CalculateStereoToMonoSize(uint32_t a_Size);

			void ConvertMonoToStereo(FILE* a_File, unsigned char*& a_DataBuffer, uint32_t a_Size, uint16_t a_BlockAlign, uint16_t bitsPerSample, uint16_t audioFormat);
			void ConvertStereoToMono(FILE* a_File, unsigned char*& a_DataBuffer, uint32_t a_Size, uint16_t a_BlockAlign, uint16_t bitsPerSample, uint16_t audioFormat);
			void ReadAsNormal(FILE* a_File, unsigned char*& a_DataBuffer, uint32_t a_Size, uint16_t a_BlockAlign, uint16_t bitsPerSample, uint16_t audioFormat);
		}
	}
}