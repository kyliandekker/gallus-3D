#include "Common.hlsl"

float4 main(PSInput input) : SV_TARGET
{
    float4 texColor = spriteTexture.Sample(samplerState, input.TEXCOORD);
    float4 finalColor = texColor;

    if (EnableLighting != 0 && DirectionalLightEnabled)
    {
        float3 normal = normalize(input.NORMAL);
        float3 lightDir = normalize(-LightDirection);
        
        float diff = max(dot(normal, lightDir), 0.0f);
        float4 diffuse = diff * LightColor * DiffuseColor;

        // Ambient Lighting
        float4 ambient = AmbientIntensity * LightColor * DiffuseColor;

        // Combine Lighting
        float4 lighting = ambient + diffuse;
        
        finalColor *= lighting;
        finalColor.a = texColor.a; // preserve original alpha
    }
    return finalColor;
}