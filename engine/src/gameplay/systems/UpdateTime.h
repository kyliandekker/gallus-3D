#pragma once

// external
#include <cstdint>

namespace gallus::gameplay
{
	enum class UpdateTime : uint32_t
	{
		UPDATE_TIME_FRAME_BEGIN = 1 << 0,
		UPDATE_TIME_FRAME = 1 << 1,
		UPDATE_TIME_FRAME_RESOLVE = 1 << 2,
		UPDATE_TIME_FRAME_APPLY = 1 << 3,
		UPDATE_TIME_FRAME_END = 1 << 4
	};
}