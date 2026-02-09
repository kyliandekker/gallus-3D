#pragma once

namespace gallus
{
	namespace utils
	{
		/// <summary>
		/// Function that reverses bytes.
		/// </summary>
		/// <param name="a_pStart">Start of the array of characters.</param>
		/// <param name="a_pSize">size of the array of characters.</param>
		/// <returns></returns>
		void reverseBytes(unsigned char* a_pStart, int a_pSize);

		/// <summary>
		/// Compares a chunk id to another chunk id.
		/// </summary>
		/// <param name="a_pChunkId">Chunk ID 1.</param>
		/// <param name="a_pChunkId2">Chunk ID 2</param>
		/// <returns></returns>
		int chunkcmp(unsigned char* a_pChunkId, unsigned char* a_pChunkId2);

		/// <summary>
		/// Compares a chunk id to another chunk id.
		/// </summary>
		/// <param name="a_pChunkId">Chunk ID 1.</param>
		/// <param name="a_pChunkId2">Chunk ID 2</param>
		/// <returns></returns>
		int chunkcmp(const char* a_pChunkId, const char* a_pChunkId2);

		/// <summary>
		/// Function that reverses bytes and casts to specified type.
		/// </summary>
		/// <param name="a_pNumber">The data that needs to be casted.</param>
		/// <returns></returns>
		template <class T>
		inline T reverseBytesC(unsigned char* a_pNumber)
		{
			reverseBytes(a_pNumber, sizeof(T));
			return *reinterpret_cast<T*>(a_pNumber);
		}
	}
}