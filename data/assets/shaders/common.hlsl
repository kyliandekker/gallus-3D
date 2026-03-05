// GENERATED FILE - DO NOT EDIT

cbuffer ShaderTransform : register(b0)
{
    float4x4 WorldViewProj;
    float4x4 WorldMatrix;
};

cbuffer TextureUV : register(b1)
{
    float4 uv;
};

cbuffer MaterialData : register(b2)
{
    uint EnableLighting;
    float3 Padding0;
    float4 DiffuseColor;
    float Metallic;
    float Smoothness;
    float2 Padding1;
};

cbuffer DirectionalLightData : register(b3)
{
    uint DirectionalLightEnabled;
    float3 LightDirection;
    float4 LightColor;
    float AmbientIntensity;
};

struct VSInput
{
    float3 POSITION : POSITION;
    float2 TEXCOORD : TEXCOORD0;
    float3 NORMAL : NORMAL;
    float4 COLOR : COLOR;
};

struct PSInput
{
    float4 POSITION : SV_POSITION;
    float2 TEXCOORD : TEXCOORD0;
    float3 NORMAL : NORMAL;
    float4 COLOR : COLOR;
};

Texture2D spriteTexture : register(t0);
SamplerState samplerState : register(s0);