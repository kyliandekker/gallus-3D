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

cbuffer Color : register(b2)
{
    float4 Color; // RGBA color multiplier
};

struct VSInput
{
    float3 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

Texture2D spriteTexture : register(t0);
SamplerState samplerState : register(s0);