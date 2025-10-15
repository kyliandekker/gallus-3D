#pragma once

#include <cstdint>

namespace gallus
{
	namespace gameplay
	{
		enum class UpdateTime : uint32_t
		{
			UPDATE_TIME_FRAME = 1 << 0,
			UPDATE_TIME_END_FRAME = 1 << 1,
			UPDATE_TIME_POST_FRAME = 1 << 2,
			UPDATE_TIME_END = 1 << 3
		};
	}
}