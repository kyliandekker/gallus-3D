#pragma once

namespace gallus
{
	constexpr int INT24_MAX = (1 << 23) - 1;
	constexpr int INT24_MIN = -INT24_MAX - 1;

	class uint24_t
	{
	protected:
		unsigned char a_aData[3] = {
			0,
			0,
			0
		};
	public:
		uint24_t() = default;

		uint24_t(int val)
		{
			operator=(val);
		}

		uint24_t& operator=(int val)
		{
			a_aData[0] = reinterpret_cast<unsigned char*>(&val)[0];
			a_aData[1] = reinterpret_cast<unsigned char*>(&val)[1];
			a_aData[2] = reinterpret_cast<unsigned char*>(&val)[2];
			return *this;
		}

		operator int() const
		{
			return (a_aData[0]) | (a_aData[1] << 8) | (a_aData[2] << 16) | (a_aData[2] & 0x80);
		}
	};
}