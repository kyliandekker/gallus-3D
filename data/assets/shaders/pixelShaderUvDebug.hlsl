Texture2D spriteTexture : register(t0);
SamplerState samplerState : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 color = spriteTexture.Sample(samplerState, input.uv);

    return float4(input.uv, 0, 1); 
}