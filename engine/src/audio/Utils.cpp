#include "Utils.h"

// external
#include <string.h>

// audio
#include "Defines.h"

namespace gallus
{
	namespace utils
	{
		void reverseBytes(unsigned char* a_pStart, int a_pSize)
		{
			unsigned char* lo = a_pStart;
			unsigned char* hi = a_pStart + a_pSize - 1;
			unsigned char swap;
			while (lo < hi)
			{
				swap = *lo;
				*lo++ = *hi;
				*hi-- = swap;
			}
		}

		int chunkcmp(unsigned char* a_pChunkId, unsigned char* a_pChunkId2)
		{
			int b = strncmp(reinterpret_cast<const char*>(a_pChunkId), reinterpret_cast<const char*>(a_pChunkId2), audio::CHUNK_ID_SIZE);
			return b;
		}

		int chunkcmp(const char* a_pChunkId, const char* a_pChunkId2)
		{
			int b = strncmp(a_pChunkId, a_pChunkId2, audio::CHUNK_ID_SIZE);
			return b;
		}
	}
}