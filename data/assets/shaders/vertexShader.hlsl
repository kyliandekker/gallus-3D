#include "Common.hlsl"

PSInput main(VSInput input)
{
    PSInput output;

    float4 worldPos = float4(input.POSITION, 1.0f);

    output.POSITION = mul(WorldViewProj, worldPos);
	output.TEXCOORD = lerp(uv.xy, uv.zw, input.TEXCOORD);
    output.NORMAL = normalize(mul((float3x3)WorldMatrix, input.NORMAL));
    output.COLOR = input.COLOR;

    return output;
}