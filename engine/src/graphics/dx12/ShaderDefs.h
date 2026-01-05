#pragma once
#include "./DX12PCH.h"

// GENERATED FILE - DO NOT EDIT

struct ShaderTransform
{
    DirectX::XMMATRIX WorldViewProj;
    DirectX::XMMATRIX WorldMatrix;
};

struct SpriteUV
{
    DirectX::XMFLOAT4 uv;
};

struct MaterialData
{
    uint32_t EnableLighting;
    DirectX::XMFLOAT3 Padding0;
    DirectX::XMFLOAT4 DiffuseColor;
    float Metallic;
    float Smoothness;
    DirectX::XMFLOAT2 Padding1;
};

struct DirectionalLightData
{
    uint32_t DirectionalLightEnabled;
    DirectX::XMFLOAT3 LightDirection;
    DirectX::XMFLOAT4 LightColor;
    float AmbientIntensity;
};

const D3D12_INPUT_ELEMENT_DESC g_aInputLayout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

enum RootParameters
{
    CBV = 0,
    SPRITE_UV = 1,
    MATERIAL = 2,
    DIRECTIONAL_LIGHT = 3,
    TEX_SRV = 4,
    SAMPLER_STATE = 5,
    NumRootParameters
};
