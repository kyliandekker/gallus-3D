#include "ConversionUtils.h"

#include <string.h>
#include <math.h>

#include "Utils.h"
#include "uint24_t.h"
#include "Defines.h"

namespace uaudio
{
	namespace wave_reader
	{
		namespace conversion
		{	// Square root of 2 (size of uint16_t)
			constexpr float SQRT_TWO = 1.4142135623730950488016887242097f;

			uint32_t Calculate8To16Size(uint32_t a_Size)
			{
				return a_Size * sizeof(uint16_t);
			}

			uint32_t Calculate8To24Size(uint32_t a_Size)
			{
				return a_Size * sizeof(uint24_t);
			}

			uint32_t Calculate8To32Size(uint32_t a_Size)
			{
				return a_Size * sizeof(uint32_t);
			}

			/// <summary>
			/// Recalculates the buffer size from 16-bit to 8-bit.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t Calculate16To8Size(uint32_t a_Size)
			{
				return a_Size / sizeof(uint16_t) * sizeof(uint8_t);
			}

			/// <summary>
			/// Recalculates the buffer size from 16-bit to 32-bit.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t Calculate16To32Size(uint32_t a_Size)
			{
				return a_Size / sizeof(uint16_t) * sizeof(uint32_t);
			}

			/// <summary>
			/// Recalculates the buffer size from 16-bit to 24-bit.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t Calculate16To24Size(uint32_t a_Size)
			{
				return a_Size / sizeof(uint16_t) * sizeof(uint24_t);
			}

			/// <summary>
			/// Recalculates the buffer size from 24-bit to 8-bit.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t Calculate24To8Size(uint32_t a_Size)
			{
				return a_Size / sizeof(uint24_t) * sizeof(uint8_t);
			}

			/// <summary>
			/// Recalculates the buffer size from 24-bit to 16-bit.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t Calculate24To16Size(uint32_t a_Size)
			{
				return a_Size / sizeof(uint24_t) * sizeof(uint16_t);
			}

			/// <summary>
			/// Recalculates the buffer size from 24-bit to 32-bit.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t Calculate24To32Size(uint32_t a_Size)
			{
				return a_Size / sizeof(uint24_t) * sizeof(uint32_t);
			}

			/// <summary>
			/// Recalculates the buffer size from 24-bit to 8-bit.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t Calculate32To8Size(uint32_t a_Size)
			{
				return a_Size / sizeof(uint32_t) * sizeof(uint8_t);
			}

			/// <summary>
			/// Recalculates the buffer size from 32-bit to 16-bit.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t Calculate32To16Size(uint32_t a_Size)
			{
				return a_Size / sizeof(uint32_t) * sizeof(uint16_t);
			}

			/// <summary>
			/// Recalculates the buffer size from 32-bit to 24-bit.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t Calculate32To24Size(uint32_t a_Size)
			{
				return a_Size / sizeof(uint32_t) * sizeof(uint24_t);
			}

			/// <summary>
			/// Recalculates the buffer size from mono to stereo.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t CalculateMonoToStereoSize(uint32_t a_Size)
			{
				return a_Size * 2;
			}

			/// <summary>
			/// Recalculates the buffer size from stereo to mono.
			/// </summary>
			/// <param name="a_Size">The buffer size.</param>
			/// <returns></returns>
			uint32_t CalculateStereoToMonoSize(uint32_t a_Size)
			{
				return a_Size / 2;
			}

			void ConvertMonoToStereo(FILE* a_File, unsigned char*& a_DataBuffer, uint32_t a_Size, uint16_t a_BlockAlign, uint16_t, uint16_t)
			{
				uint32_t mono_size = a_Size / 2;
				uint32_t mono_blockAlign = a_BlockAlign / 2;
				for (uint32_t i = 0; i < mono_size; i += mono_blockAlign)
				{
					fread(a_DataBuffer, mono_blockAlign, 1, a_File);
					memcpy(utils::add(a_DataBuffer, mono_blockAlign), a_DataBuffer, mono_blockAlign);
					a_DataBuffer += a_BlockAlign;
				}
			}

			void ConvertStereoToMono(FILE* a_File, unsigned char*& a_DataBuffer, uint32_t a_Size, uint16_t a_BlockAlign, uint16_t bitsPerSample, uint16_t audioFormat)
			{
				size_t realNumSamples = a_Size / a_BlockAlign;

				if (realNumSamples == 0)
					return;

				int index = 0;
				for (size_t i = 0; i < realNumSamples; i++)
				{
					long pos = ftell(a_File);
					if (bitsPerSample == WAVE_BITS_PER_SAMPLE_8)
					{
						uint8_t luSample = 0;
						fread(&luSample, sizeof(luSample), 1, a_File);
						int8_t lSample = INT8_MAX - luSample;

						uint8_t ruSample = 0;
						fread(&ruSample, sizeof(luSample), 1, a_File);
						int8_t rSample = INT8_MAX - ruSample;

						double l = static_cast<double>(lSample);
						double r = static_cast<double>(rSample);
						double b = (l * 0.5) + (r / 2.0);

						if (b > 0.0)
							b = floor(b);
						else
							b = ceil(b);

						int8_t fullS = static_cast<int8_t>(b);
						fullS *= UINT8_MAX;

						uint8_t iSample = INT8_MAX + fullS;

						unsigned char d = iSample;
						a_DataBuffer[index] = d;
						index++;
					}
					else if (bitsPerSample == WAVE_BITS_PER_SAMPLE_16)
					{
						int16_t lSample = 0;
						fread(&lSample, a_BlockAlign, 1, a_File);
						int16_t rSample = 0;
						fread(&rSample, a_BlockAlign, 1, a_File);

						double fLSample = static_cast<double>(lSample) / INT16_MAX;
						double fRSample = static_cast<double>(rSample) / INT16_MAX;

						double sample = (fLSample / 2) + (fRSample / 2);
						sample *= INT16_MAX;

						int16_t iSample = static_cast<int16_t>(sample);

						for (size_t j = 0; j < a_BlockAlign; j++)
						{
							unsigned char d = *reinterpret_cast<unsigned char*>(utils::add(&iSample, j));
							a_DataBuffer[index] = d;
							index++;
						}
					}
					else if (bitsPerSample == WAVE_BITS_PER_SAMPLE_32_FLOAT && audioFormat == WAV_FORMAT_IEEE_FLOAT)
					{
						float lSample = 0;
						fread(&lSample, a_BlockAlign, 1, a_File);
						float rSample = 0;
						fread(&rSample, a_BlockAlign, 1, a_File);

						double fLSample = static_cast<double>(lSample);
						double fRSample = static_cast<double>(rSample);

						double sample = (fLSample * 0.5) + (fRSample * 0.5);

						float iSample = static_cast<float>(sample);

						for (size_t j = 0; j < a_BlockAlign; j++)
						{
							unsigned char d = *reinterpret_cast<unsigned char*>(utils::add(&iSample, j));
							a_DataBuffer[index] = d;
							index++;
						}
					}
					else if (bitsPerSample == WAVE_BITS_PER_SAMPLE_64)
					{
						double lSample = 0;
						fread(&lSample, a_BlockAlign, 1, a_File);
						double rSample = 0;
						fread(&rSample, a_BlockAlign, 1, a_File);

						double fLSample = static_cast<double>(lSample);
						double fRSample = static_cast<double>(rSample);

						double sample = (fLSample * 0.5) + (fRSample * 0.5);

						double iSample = static_cast<double>(sample);

						for (size_t j = 0; j < a_BlockAlign; j++)
						{
							unsigned char d = *reinterpret_cast<unsigned char*>(utils::add(&iSample, j));
							a_DataBuffer[index] = d;
							index++;
						}
					}
					else if (bitsPerSample == WAVE_BITS_PER_SAMPLE_32)
					{
						int32_t lSample = 0;
						fread(&lSample, a_BlockAlign, 1, a_File);
						int32_t rSample = 0;
						fread(&rSample, a_BlockAlign, 1, a_File);

						double fLSample = static_cast<double>(lSample) / INT32_MAX;
						double fRSample = static_cast<double>(rSample) / INT32_MAX;

						double sample = (fLSample * 0.5) + (fRSample * 0.5);
						sample *= INT32_MAX;

						int32_t iSample = static_cast<int32_t>(sample);

						for (size_t j = 0; j < a_BlockAlign; j++)
						{
							unsigned char d = *reinterpret_cast<unsigned char*>(utils::add(&iSample, j));
							a_DataBuffer[index] = d;
							index++;
						}
					}
					fseek(a_File, pos, SEEK_SET);
					fseek(a_File, a_BlockAlign * 2, SEEK_CUR);
				}
			}

			void ReadAsNormal(FILE* a_File, unsigned char*& a_DataBuffer, uint32_t a_Size, uint16_t, uint16_t, uint16_t)
			{
				fread(a_DataBuffer, a_Size, 1, a_File);
			}

			///// <summary>
			///// Converts 24 bit pcm data to 16 bit pcm data.
			///// </summary>
			///// <param name="a_DataBuffer">The new data buffer.</param>
			///// <param name="a_OriginalDataBuffer">The original data buffer.</param>
			///// <param name="a_Size">The data size (will get changed)</param>
			//UAUDIO_WAVE_READER_CONVERSION_RESULT Convert24To16(unsigned char* a_DataBuffer, unsigned char* a_OriginalDataBuffer, uint32_t& a_Size)
			//{
			//	// Determine the size of a 16bit data array.
			//	// Chunk size divided by the size of a 24bit int (3) multiplied by the size of a 16bit int (2).
			//	const uint32_t new_size = Calculate24To16Size(a_Size);

			//	uint16_t* array_16 = reinterpret_cast<uint16_t*>(a_DataBuffer);

			//	int i = 0;
			//	for (uint32_t a = 0; a < a_Size; a += sizeof(uint24_t), i++)
			//	{
			//		// Skip 1 bit every time since we go from 24bit to 16bit.
			//		array_16[i] = *reinterpret_cast<uint16_t*>(a_OriginalDataBuffer + a + 1);
			//	}
			//	a_Size = new_size;
			//	return UAUDIO_WAVE_READER_CONVERSION_RESULT::UAUDIO_OK;
			//}

			///// <summary>
			///// Converts 32 bit pcm data to 16 bit pcm data.
			///// </summary>
			///// <param name="a_DataBuffer">The new data buffer.</param>
			///// <param name="a_OriginalDataBuffer">The original data buffer.</param>
			///// <param name="a_Size">The data size (will get changed)</param>
			//UAUDIO_WAVE_READER_CONVERSION_RESULT Convert32To16(unsigned char* a_DataBuffer, unsigned char* a_OriginalDataBuffer, uint32_t& a_Size)
			//{
			//	// Determine the size of a 16bit data array.
			//	// Chunk size divided by the size of a 32bit int (4) multiplied by the size of a 16bit int (2).
			//	uint32_t new_size = Calculate32To16Size(a_Size);

			//	uint16_t* array_16 = reinterpret_cast<uint16_t*>(a_DataBuffer);

			//	int i = 0;
			//	for (uint32_t a = 0; a < a_Size; a += sizeof(uint32_t), i++)
			//	{
			//		// Add the size of a 32bit int (4) to move the data pointer.
			//		float converted_value = *reinterpret_cast<float*>(a_OriginalDataBuffer + a);

			//		// Calc 32 to 16 bit int.
			//		converted_value /= SQRT_TWO;
			//		converted_value *= INT16_MAX;

			//		array_16[i] = static_cast<uint16_t>(converted_value);
			//	}
			//	a_Size = new_size;
			//	return UAUDIO_WAVE_READER_CONVERSION_RESULT::UAUDIO_OK;
			//}

			//// UAUDIO_WAVE_READER_CONVERSION_RESULT ConvertToSamples(float* a_OutSamples, unsigned char* a_DataBuffer, uint32_t a_SampleCount)
			////{
			////	for (uint32_t i = 0; i < a_SampleCount; ++i, a_DataBuffer += 4, ++a_OutSamples)
			////	{
			////		short lSample = *(const short*)a_DataBuffer;
			////		short rSample = *(const short*)(a_DataBuffer + 2);
			////		*a_OutSamples = ((float)lSample + (float)rSample) / 65536.f;
			////	}
			////	return UAUDIO_WAVE_READER_CONVERSION_RESULT::UAUDIO_OK;
			//// }

			///// <summary>
			///// Calculates the buffer size and truncates it if it overflows.
			///// </summary>
			///// <param name="a_StartingPoint">The position of the original data pointer.</param>
			///// <param name="a_ElementCount">The buffer size from the starting point.</param>
			///// <param name="a_ChunkSize">The total chunk size of the data pointer.</param>
			///// <param name="a_BufferSize">The buffer size that will be changed.</param>
			///// <returns></returns>
			//UAUDIO_WAVE_READER_CONVERSION_RESULT CalculateBufferSize(uint32_t a_StartingPoint, uint32_t a_ElementCount, uint32_t a_ChunkSize, uint32_t& a_BufferSize)
			//{
			//	if (a_StartingPoint + a_ElementCount >= a_ChunkSize)
			//	{
			//		uint32_t new_size = a_ElementCount - ((a_StartingPoint + a_ElementCount) - a_ChunkSize);
			//		if (new_size <= 0)
			//			new_size = 0;

			//		a_BufferSize = new_size;
			//	}
			//	return UAUDIO_WAVE_READER_CONVERSION_RESULT::UAUDIO_OK;
			//}

			///// <summary>
			///// Reads a data buffer and copies its contents from starting position to starting position + element count.
			///// </summary>
			///// <param name="a_StartingPoint">The position of the original data pointer.</param>
			///// <param name="a_ElementCount">The buffer size from the starting point.</param>
			///// <param name="a_DataBuffer">The new data buffer.</param>
			///// <param name="a_OriginalDataBuffer">The data buffer with the original data.</param>
			///// <returns></returns>
			//UAUDIO_WAVE_READER_CONVERSION_RESULT ReadDataBuffer(uint32_t a_StartingPoint, const uint32_t& a_ElementCount, unsigned char* a_DataBuffer, unsigned char* a_OriginalDataBuffer)
			//{
			//	memcpy(a_DataBuffer, utils::add(a_OriginalDataBuffer, a_StartingPoint), a_ElementCount);
			//	return UAUDIO_WAVE_READER_CONVERSION_RESULT::UAUDIO_OK;
			//}

		}
	}
}