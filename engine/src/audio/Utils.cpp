#include "Utils.h"
#include <string.h>
#include "Defines.h"

namespace uaudio
{
	namespace utils
	{
		void reverseBytes(unsigned char* start, int size)
		{
			unsigned char* lo = start;
			unsigned char* hi = start + size - 1;
			unsigned char swap;
			while (lo < hi)
			{
				swap = *lo;
				*lo++ = *hi;
				*hi-- = swap;
			}
		}

		int chunkcmp(unsigned char* chunkID1, const char* chunkID2)
		{
			int b = strncmp(reinterpret_cast<const char*>(chunkID1), chunkID2, wave_reader::CHUNK_ID_SIZE);
			return b;
		}

		int chunkcmp(unsigned char* chunkID1, unsigned char* chunkID2)
		{
			int b = strncmp(reinterpret_cast<const char*>(chunkID1), reinterpret_cast<const char*>(chunkID2), wave_reader::CHUNK_ID_SIZE);
			return b;
		}
	}
}