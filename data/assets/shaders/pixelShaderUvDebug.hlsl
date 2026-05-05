#include "Common.hlsl"

float4 main(PSInput input) : SV_TARGET
{
    return float4(input.TEXCOORD, 0, 1); 
}