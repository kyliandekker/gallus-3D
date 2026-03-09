#include "Common.hlsl"

PSInput main(VSInput input)
{
    PSInput output;

    float4 pos = float4(input.POSITION, 1.0f);
    float3 normal = input.NORMAL;

    if (input.WEIGHTS.x + input.WEIGHTS.y + input.WEIGHTS.z + input.WEIGHTS.w > 0.0f)
    {
        pos = mul(Bones[input.JOINTS.x], float4(input.POSITION,1)) * input.WEIGHTS.x +
              mul(Bones[input.JOINTS.y], float4(input.POSITION,1)) * input.WEIGHTS.y +
              mul(Bones[input.JOINTS.z], float4(input.POSITION,1)) * input.WEIGHTS.z +
              mul(Bones[input.JOINTS.w], float4(input.POSITION,1)) * input.WEIGHTS.w;

        normal = mul((float3x3)Bones[input.JOINTS.x], input.NORMAL) * input.WEIGHTS.x +
                 mul((float3x3)Bones[input.JOINTS.y], input.NORMAL) * input.WEIGHTS.y +
                 mul((float3x3)Bones[input.JOINTS.z], input.NORMAL) * input.WEIGHTS.z +
                 mul((float3x3)Bones[input.JOINTS.w], input.NORMAL) * input.WEIGHTS.w;
    }

    output.POSITION = mul(WorldViewProj, pos);
    output.NORMAL = normalize(normal); // skip WorldMatrix for bind pose
    output.TEXCOORD = lerp(uv.xy, uv.zw, input.TEXCOORD);
    output.COLOR = input.COLOR;

    return output;
}