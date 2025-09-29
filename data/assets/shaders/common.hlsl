// common.hlsl

// Transform matrices
cbuffer Transform : register(b0)
{
    float4x4 WorldViewProj;
};

// Sprite UV
cbuffer SpriteUV : register(b1)
{
    float4 uv; // xy = uv0, zw = uv1
};

struct VSInput
{
    float3 pos : POSITION;
    float2 uv  : TEXCOORD0;
	float3 normal   : NORMAL;
    float4 color    : COLOR;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
	float3 normal   : NORMAL;
    float4 color    : COLOR;
};

Texture2D spriteTexture : register(t0);
SamplerState samplerState : register(s0);