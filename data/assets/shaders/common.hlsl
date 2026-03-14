// GENERATED FILE - DO NOT EDIT

// BUFFERS

cbuffer ShaderTransform : register(b0)
{
	float4x4 WorldViewProj; // 64 bytes.
	float4x4 WorldMatrix; // 64 bytes.
}; // total size = 128.

cbuffer TextureUV : register(b1)
{
	float4 uv; // 16 bytes.
}; // total size = 16.

cbuffer MaterialData : register(b2)
{
	uint EnableLighting; // 4 bytes.
	float3 Padding0; // 12 bytes.
	float4 DiffuseColor; // 16 bytes.
	float Metallic; // 4 bytes.
	float Smoothness; // 4 bytes.
	float2 Padding1; // 8 bytes.
}; // total size = 48.

cbuffer DirectionalLightData : register(b3)
{
	uint DirectionalLightEnabled; // 4 bytes.
	float3 LightDirection; // 12 bytes.
	float4 LightColor; // 16 bytes.
	float AmbientIntensity; // 4 bytes.
	float Padding2[3]; // 12 bytes.
}; // total size = 48.

cbuffer SkinningData : register(b4)
{
	float4x4 Bones[64]; // 4096 bytes.
}; // total size = 4096.

// VSINPUT

struct VSInput
{
	float3 POSITION : POSITION; // 12 bytes.
	float2 TEXCOORD : TEXCOORD0; // 8 bytes.
	float3 NORMAL : NORMAL; // 12 bytes.
	uint4 JOINTS : JOINTS; // 16 bytes.
	float4 WEIGHTS : WEIGHTS; // 16 bytes.
}; // total size = 64.

// PSINPUT

struct PSInput
{
	float4 POSITION : SV_POSITION; // 16 bytes.
	float2 TEXCOORD : TEXCOORD0; // 8 bytes.
	float3 NORMAL : NORMAL; // 12 bytes.
	float4 COLOR : COLOR; // 16 bytes.
}; // total size = 52.
// TODO: Alignment is not correct for GPU. Fix!!! (52 bytes total, missing 12 bytes)

Texture2D spriteTexture : register(t0);
SamplerState samplerState : register(s0);