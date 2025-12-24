#include "Common.hlsl"

float3 DegreesToDirection(float3 degrees)
{
    float3 radians = degrees * (3.14159265f / 180.0f);
    
    float cosPitch = cos(radians.y);
    return normalize(float3(
        cos(radians.x) * cosPitch, // X
        sin(radians.y),             // Y
        sin(radians.x) * cosPitch   // Z
    ));
}

float4 main(PSInput input) : SV_TARGET
{
    float4 texColor = spriteTexture.Sample(samplerState, input.TEXCOORD);
    float4 finalColor = texColor;

    if (EnableLighting != 0 && DirectionalLightEnabled)
    {
        float3 normal = normalize(input.NORMAL);
        float3 lightDir = -DegreesToDirection(LightDirection); 
        
        float diff = max(dot(normal, lightDir), 0.0f);
        float4 diffuse = diff * LightColor * DiffuseColor;

        // Ambient Lighting
        float4 ambient = AmbientIntensity * LightColor * DiffuseColor;

        // Combine Lighting
        float4 lighting = ambient + diffuse;
        
        finalColor *= lighting;
    }
    return finalColor;
}