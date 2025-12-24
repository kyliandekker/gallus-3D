#include "Common.hlsl"

float4 main(PSInput input) : SV_TARGET
{
    float4 color = spriteTexture.Sample(samplerState, input.TEXCOORD);

    color *= DiffuseColor;
    
    return color; 
}