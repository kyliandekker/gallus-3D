#pragma once

#include <cstdint>

namespace gallus
{
	namespace core
	{
#define _KB(x) (x * 1024)
#define _MB(x) (x * 1024 * 1024)

#define _64KB _KB(64)
#define _1MB _MB(1)
#define _2MB _MB(2)
#define _4MB _MB(4)
#define _8MB _MB(8)
#define _16MB _MB(16)
#define _32MB _MB(32)
#define _64MB _MB(64)
#define _128MB _MB(128)
#define _256MB _MB(256)

		/// <summary>
		/// Adds specific size to a pointer.
		/// </summary>
		/// <param name="a_Ptr">The pointer that will be offset.</param>
		/// <param name="a_Size">The amount that the pointer will be offset with.</param>
		/// <returns>A pointer with the new offset.</returns>
		inline void* add(void* a_Ptr, size_t a_Size)
		{
			return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(a_Ptr) + a_Size);
		}

		/// <summary>
		/// Adds specific size to a pointer and casts it to a specified type.
		/// </summary>
		/// <typeparam name="T">The type the pointer will be cast to.</typeparam>
		/// <param name="a_Ptr">The pointer that will be offset.</param>
		/// <param name="a_Size">The amount that the pointer will be offset with.</param>
		/// <returns>A pointer of type T with the new offset.</returns>
		template<typename T>
		inline T* addAs(T* a_Ptr, size_t a_Size)
		{
			return reinterpret_cast<T*>(add(a_Ptr, a_Size));
		}
	}
}