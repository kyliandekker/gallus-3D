#include "Common.hlsl"

PSInput main(VSInput input)
{
    PSInput output;

    float4 pos = float4(input.POSITION, 1.0f);
    float3 normal = input.NORMAL;

    output.POSITION = mul(WorldViewProj, pos);
    output.NORMAL = normalize(normal); // skip WorldMatrix for bind pose
    output.TEXCOORD = lerp(uv.xy, uv.zw, input.TEXCOORD);
    output.COLOR = input.COLOR;

    return output;
}