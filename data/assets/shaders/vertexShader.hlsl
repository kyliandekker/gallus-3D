cbuffer Transform : register(b0)
{
    float4x4 WorldViewProj;
}

cbuffer SpriteUV : register(b1)
{
    float4 uv; // xy = uv0, zw = uv1
}

struct VSInput
{
    float2 pos : POSITION; 
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

PSInput main(VSInput input)
{
    PSInput output;

    float4 worldPos = float4(input.pos, 0.0f, 1.0f);

    output.pos = mul(WorldViewProj, worldPos);

	output.uv = lerp(uv.xy, uv.zw, input.uv);

    return output;
}