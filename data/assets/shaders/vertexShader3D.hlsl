#include "Common.hlsl"

PSInput main(VSInput input)
{
    PSInput output;

    float4 worldPos = float4(input.pos, 1.0f);

    output.pos = mul(WorldViewProj, worldPos);
	output.uv = lerp(uv.xy, uv.zw, input.uv);
    output.normal = normalize(mul((float3x3)WorldViewProj, input.normal));
    output.color = input.color;

    return output;
}