#include "Common.hlsl"

PSInput main(VSInput input)
{
    PSInput output;

    float4 worldPos = float4(input.pos, 0.0f, 1.0f);

    output.pos = mul(WorldViewProj, worldPos);

	output.uv = lerp(uv.xy, uv.zw, input.uv);

    return output;
}