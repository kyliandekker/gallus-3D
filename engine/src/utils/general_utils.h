#pragma once

namespace gallus
{
	namespace utils
	{
		template <typename T>
		T clamp(T current, T min, T max)
		{
			if (current > max)
			{
				current = max;
			}
			else if (current < min)
			{
				current = min;
			}
			return current;
		}
	}
}