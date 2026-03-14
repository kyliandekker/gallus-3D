#pragma once

#include "./DX12PCH.h"

// GENERATED FILE - DO NOT EDIT

namespace gallus::graphics::dx12
{
	// BUFFERS

	struct ShaderTransform // registered at b0.
	{
		DirectX::XMMATRIX WorldViewProj; // 64 bytes.
		DirectX::XMMATRIX WorldMatrix; // 64 bytes.
	}; // total size = 128.

	struct TextureUV // registered at b1.
	{
		DirectX::XMFLOAT4 uv; // 16 bytes.
	}; // total size = 16.

	struct MaterialData // registered at b2.
	{
		uint32_t EnableLighting; // 4 bytes.
		DirectX::XMFLOAT3 Padding0; // 12 bytes.
		DirectX::XMFLOAT4 DiffuseColor; // 16 bytes.
		float Metallic; // 4 bytes.
		float Smoothness; // 4 bytes.
		DirectX::XMFLOAT2 Padding1; // 8 bytes.
	}; // total size = 48.

	struct DirectionalLightData // registered at b3.
	{
		uint32_t DirectionalLightEnabled; // 4 bytes.
		DirectX::XMFLOAT3 LightDirection; // 12 bytes.
		DirectX::XMFLOAT4 LightColor; // 16 bytes.
		float AmbientIntensity; // 4 bytes.
		float Padding2[3]; // 12 bytes.
	}; // total size = 48.

	struct SkinningData // registered at b4.
	{
		DirectX::XMMATRIX Bones[64]; // 4096 bytes.
	}; // total size = 4096.

	// VSInput

	struct VSInput
	{
		DirectX::XMFLOAT3 POSITION;
		DirectX::XMFLOAT2 TEXCOORD;
		DirectX::XMFLOAT3 NORMAL;
		DirectX::XMUINT4 JOINTS;
		DirectX::XMFLOAT4 WEIGHTS;
	};

	// INPUT

	const D3D12_INPUT_ELEMENT_DESC g_aInputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "JOINTS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// ROOT PARAMETERS

	enum RootParameters
	{
		TRANSFORM = 0, // registered at b0.
		SPRITE_UV = 1, // registered at b1.
		MATERIAL = 2, // registered at b2.
		DIRECTIONAL_LIGHT = 3, // registered at b3.
		SKINNING_DATA = 4, // registered at b4.
		TEX_SRV = 5, // registered at t0.
		SAMPLER_STATE = 6, // registered at s0.
		NumRootParameters = 8,
	};
}