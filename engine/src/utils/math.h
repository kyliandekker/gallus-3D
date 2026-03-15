#pragma once

#include "graphics/dx12/DX12PCH.h"

namespace gallus
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

	// BaseVector2
	template <typename T>
	class BaseVector2
	{
	public:
		T x = {}, y = {};

		BaseVector2() = default;
		BaseVector2(T x, T y) : x(x), y(y)
		{}

		BaseVector2<T> operator+(const BaseVector2<T>& other) const
		{
			return { x + other.x, y + other.y };
		}

		BaseVector2<T> operator-(const BaseVector2<T>& other) const
		{
			return { x - other.x, y - other.y };
		}

		BaseVector2<T> operator*(T scalar) const
		{
			return { x * scalar, y * scalar };
		}

		BaseVector2<T> operator/(T scalar) const
		{
			return { x / scalar, y / scalar };
		}
	};

	// Vector2
	class Vector2 : public BaseVector2<float>
	{
	public:
		Vector2() = default;
		Vector2(float x, float y) : BaseVector2(x, y)
		{}
		Vector2(const DirectX::XMFLOAT2& v) : BaseVector2(v.x, v.y)
		{}

		operator DirectX::XMFLOAT2() const
		{
			return { x, y };
		}

		DirectX::XMFLOAT2 DX12Vector() const
		{
			return { x, y };
		}
	};

	// IVector2
	class IVector2 : public BaseVector2<int>
	{
	public:
		IVector2() = default;
		IVector2(int x, int y) : BaseVector2(x, y)
		{}
		IVector2(const DirectX::XMINT2& v) : BaseVector2(v.x, v.y)
		{}

		operator DirectX::XMINT2() const
		{
			return { x, y };
		}
		DirectX::XMINT2 DX12Vector() const
		{
			return { x, y };
		}
	};

	// BaseVector3
	template <typename T>
	class BaseVector3 : public BaseVector2<T>
	{
	public:
		T z = {};

		BaseVector3() = default;
		BaseVector3(T x, T y, T z) : BaseVector2<T>(x, y), z(z)
		{}

		BaseVector3<T> operator+(const BaseVector3<T>& other) const
		{
			return { this->x + other.x, this->y + other.y, z + other.z };
		}

		BaseVector3<T> operator-(const BaseVector3<T>& other) const
		{
			return { this->x - other.x, this->y - other.y, z - other.z };
		}

		BaseVector3<T> operator*(T scalar) const
		{
			return { this->x * scalar, this->y * scalar, z * scalar };
		}

		BaseVector3<T> operator/(T scalar) const
		{
			return { this->x / scalar, this->y / scalar, z / scalar };
		}
	};

	// Vector3
	class Vector3 : public BaseVector3<float>
	{
	public:
		Vector3() = default;
		Vector3(float x, float y, float z) : BaseVector3<float>(x, y, z)
		{}
		Vector3(const DirectX::XMFLOAT3& v) : BaseVector3<float>(v.x, v.y, v.z)
		{}

		Vector3(const class Vector4& v4);

		operator DirectX::XMFLOAT3() const
		{
			return { x, y, z };
		}
		DirectX::XMFLOAT3 DX12Vector() const
		{
			return { x, y, z };
		}

		// Indexed access
		float& operator[](int index)
		{
			switch (index)
			{
				case 0: return x;
				case 1: return y;
				case 2: return z;
				default: return x;
			}
		}

		const float& operator[](int index) const
		{
			switch (index)
			{
				case 0: return x;
				case 1: return y;
				case 2: return z;
				default: return x;
			}
		}
	};

	// BaseVector4
	template <typename T>
	class BaseVector4 : public BaseVector3<T>
	{
	public:
		T w = {};

		BaseVector4() = default;
		BaseVector4(T x, T y, T z, T w) : BaseVector3<T>(x, y, z), w(w)
		{}

		BaseVector4<T> operator+(const BaseVector4<T>& other) const
		{
			return { this->x + other.x, this->y + other.y, this->z + other.z, w + other.w };
		}

		BaseVector4<T> operator-(const BaseVector4<T>& other) const
		{
			return { this->x - other.x, this->y - other.y, this->z - other.z, w - other.w };
		}

		BaseVector4<T> operator*(T scalar) const
		{
			return { this->x * scalar, this->y * scalar, this->z * scalar, w * scalar };
		}

		BaseVector4<T> operator/(T scalar) const
		{
			return { this->x / scalar, this->y / scalar, this->z / scalar, w / scalar };
		}
	};

	// Vector4
	class Vector4 : public BaseVector4<float>
	{
	public:
		Vector4() = default;
		Vector4(float x, float y, float z, float w) : BaseVector4<float>(x, y, z, w)
		{}
		Vector4(const DirectX::XMFLOAT4& v) : BaseVector4<float>(v.x, v.y, v.z, v.w)
		{}
		Vector4(const Vector3& v, float w = 0.0f) : BaseVector4<float>(v.x, v.y, v.z, w)
		{}

		operator DirectX::XMFLOAT4() const
		{
			return { x, y, z, w };
		}
		DirectX::XMFLOAT4 DX12Vector() const
		{
			return { x, y, z, w };
		}
	};

	inline Vector3::Vector3(const Vector4& v4) : BaseVector3<float>(v4.x, v4.y, v4.z)
	{}

// ----------------------------
// Color classes
// ----------------------------
	class Color3
	{
	public:
		float r = 0.0f, g = 0.0f, b = 0.0f;

		Color3() = default;
		Color3(float r, float g, float b) : r(r), g(g), b(b)
		{}
		Color3(unsigned char r, unsigned char g, unsigned char b)
		{
			this->r = r / 255.0f;
			this->g = g / 255.0f;
			this->b = b / 255.0f;
		}

		// Convert to 0-255 integers
		unsigned char R() const
		{
			return static_cast<unsigned char>(clamp(r, 0.0f, 1.0f) * 255.0f);
		}
		unsigned char G() const
		{
			return static_cast<unsigned char>(clamp(g, 0.0f, 1.0f) * 255.0f);
		}
		unsigned char B() const
		{
			return static_cast<unsigned char>(clamp(b, 0.0f, 1.0f) * 255.0f);
		}

		operator Vector3() const
		{
			return { r, g, b };
		}
		operator DirectX::XMFLOAT3() const
		{
			return { r, g, b };
		}
	};

	class Color4
	{
	public:
		float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;

		Color4() = default;
		Color4(const DirectX::XMFLOAT4& v) : r(v.x), g(v.y), b(v.z), a(v.w)
		{}
		Color4(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a)
		{}
		Color4(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
		{
			this->r = r / 255.0f;
			this->g = g / 255.0f;
			this->b = b / 255.0f;
			this->a = a / 255.0f;
		}

		unsigned char R() const
		{
			return static_cast<unsigned char>(clamp(r, 0.0f, 1.0f) * 255.0f);
		}
		unsigned char G() const
		{
			return static_cast<unsigned char>(clamp(g, 0.0f, 1.0f) * 255.0f);
		}
		unsigned char B() const
		{
			return static_cast<unsigned char>(clamp(b, 0.0f, 1.0f) * 255.0f);
		}
		unsigned char A() const
		{
			return static_cast<unsigned char>(clamp(a, 0.0f, 1.0f) * 255.0f);
		}

		operator Vector4() const
		{
			return { r, g, b, a };
		}
		operator DirectX::XMFLOAT4() const
		{
			return { r, g, b, a };
		}
	};
}