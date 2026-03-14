#include "Common.hlsl"

PSInput main(VSInput input)
{
    PSInput output;

    float4 pos = float4(input.POSITION, 1.0f);
    float3 normal = input.NORMAL;

    float totalWeight = input.WEIGHTS.x + input.WEIGHTS.y + input.WEIGHTS.z + input.WEIGHTS.w;
    
    output.POSITION = mul(WorldViewProj, pos);
    output.NORMAL = normalize(mul((float3x3)WorldMatrix, normal));
    output.TEXCOORD = lerp(uv.xy, uv.zw, input.TEXCOORD);

    return output;
}