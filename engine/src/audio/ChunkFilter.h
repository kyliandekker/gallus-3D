#pragma once

#include <cstdint>

namespace gallus
{
	namespace audio
	{
		/*
		 * This is a filter for the wave file loader. If you want to only get specific chunks, use this.
		*/
		class ChunkFilter
		{
		public:

			ChunkFilter()
			{ }

			ChunkFilter(const char* a_pData, size_t a_iSize);

			const char* data() const;
			size_t size() const;

			const char* operator [](size_t i) const;
		private:
			size_t m_iSize = 0;
			const char* m_pData = nullptr;
		};
	}
}