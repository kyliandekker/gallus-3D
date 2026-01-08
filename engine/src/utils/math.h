#pragma once

#include "graphics/dx12/DX12PCH.h"

#ifdef _EDITOR
#include <imgui/imgui.h>
#endif

namespace gallus
{
	template <typename T>
	class BaseVector2
	{
	public:
		BaseVector2() = default;
		BaseVector2(T x, T y) : x(x), y(y)
		{}

		T x, y;
	};
	
	class Vector2 : public BaseVector2<float>
	{
	public:
		Vector2() = default;
		Vector2(float x, float y) : BaseVector2(x, y)
		{}

		Vector2(const DirectX::XMFLOAT2& a_vVector) : BaseVector2(a_vVector.x, a_vVector.y)
		{}

		Vector2(const ImVec2& a_vVector) : BaseVector2(a_vVector.x, a_vVector.y)
		{}

		DirectX::XMFLOAT2 DX12Vector() const
		{
			return { x, y };
		}
		ImVec2 ImVector() const
		{
			return { x, y };
		}
	};
	
	class IVector2 : public BaseVector2<int>
	{
	public:
		IVector2() = default;
		IVector2(int x, int y) : BaseVector2(x, y)
		{}

		IVector2(const DirectX::XMINT2& a_vVector) : BaseVector2(a_vVector.x, a_vVector.y)
		{}

		IVector2(const ImVec2& a_vVector) : BaseVector2(static_cast<int>(a_vVector.x), static_cast<int>(a_vVector.y))
		{}

		DirectX::XMINT2 DX12Vector() const
		{
			return { x, y };
		}
		DirectX::XMFLOAT2 DX12VectorF() const
		{
			return { static_cast<float>(x), static_cast<float>(y) };
		}
		ImVec2 ImVector() const
		{
			return { static_cast<float>(x), static_cast<float>(y) };
		}
	};
	
	//template <typename T>
	//class BaseVector3 : public BaseVector2<T>
	//{
	//public:
	//	BaseVector3(T x, T y, T z) : BaseVector2(x, y), z(z)
	//	{}

	//	T z;
	//};
	//
	//class Vector3 : public BaseVector3<float>
	//{
	//public:
	//	Vector3(float x, float y, float z) : BaseVector3(x, y, z)
	//	{}

	//	Vector3(const DirectX::XMFLOAT3& a_vVector) : BaseVector3(a_vVector.x, a_vVector.y, a_vVector.z)
	//	{}

	//	DirectX::XMFLOAT3 DX12Vector() const
	//	{
	//		return { x, y, z };
	//	}
	//};
}