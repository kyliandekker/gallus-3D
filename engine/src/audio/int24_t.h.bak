#pragma once

namespace gallus
{
    namespace audio
    {
        constexpr int INT24_MAX = (1 << 23) - 1;
        constexpr int INT24_MIN = -INT24_MAX - 1;

#pragma pack(push, 1)
        class int24_t
        {
        protected:
            unsigned char a_aData[3] = { 0,0,0 };
        public:
            int24_t() {}
            int24_t(int val) { operator=(val); }
            int24_t& operator=(int val) { a_aData[0] = static_cast<uint8_t>(val); a_aData[1] = static_cast<uint8_t>(val >> 8); a_aData[2] = static_cast<uint8_t>(val >> 16); a_aData[2] |= static_cast<uint8_t>(val >> 31 << 7); return *this; }
            operator int() const { return (a_aData[0]) | (a_aData[1] << 8) | (a_aData[2] << 16) | (a_aData[2] & 0x80 ? -1 << 24 : 0); }
            int24_t& operator++() { operator=(*this + 1); return *this; }
            int24_t operator++(int) { int24_t t = *this; operator=(*this + 1); return t; }
            int24_t& operator--() { operator=(*this + 1); return *this; }
            int24_t operator--(int) { int24_t t = *this; operator=(*this + 1); return t; }
            int24_t& operator+=(int rhs) { operator=(*this + rhs); return *this; }
            int24_t& operator-=(int rhs) { operator=(*this - rhs); return *this; }
            int24_t& operator*=(int rhs) { operator=(*this * rhs); return *this; }
            int24_t& operator/=(int rhs) { operator=(*this / rhs); return *this; }
            int24_t& operator%=(int rhs) { operator=(*this % rhs); return *this; }
            int24_t& operator>>=(int rhs) { operator=(*this >> rhs); return *this; }
            int24_t& operator<<=(int rhs) { operator=(*this << rhs); return *this; }
            int24_t& operator|=(int rhs) { operator=(*this | rhs); return *this; }
            int24_t& operator&=(int rhs) { operator=(*this & rhs); return *this; }
            int24_t& operator^=(int rhs) { operator=(*this ^ rhs); return *this; }
            int24_t& operator|=(int24_t rhs) { a_aData[0] |= rhs.a_aData[0]; a_aData[1] |= rhs.a_aData[1]; a_aData[2] |= rhs.a_aData[2]; return *this; }
            int24_t& operator&=(int24_t rhs) { a_aData[0] &= rhs.a_aData[0]; a_aData[1] &= rhs.a_aData[1]; a_aData[2] &= rhs.a_aData[2]; return *this; }
            int24_t& operator^=(int24_t rhs) { a_aData[0] ^= rhs.a_aData[0]; a_aData[1] ^= rhs.a_aData[1]; a_aData[2] ^= rhs.a_aData[2]; return *this; }
        };
#pragma pack(pop)
    }
}