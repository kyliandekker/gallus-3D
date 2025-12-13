#include "Common.hlsl"

float4 main(PSInput input) : SV_TARGET
{
    float4 color = spriteTexture.Sample(samplerState, input.uv);

    color *= Color;
    
    return color; 
}