cbuffer Transform : register(b0) 
{
    float4x4 WorldViewProj;
}

cbuffer SpriteUV  : register(b1) 
{
    float4 uv; // keep for signature compatibility
}

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

VSOutput main(uint vertexID : SV_VertexID)
{
    VSOutput o;

    // Vertex positions for a fullscreen quad using TRIANGLESTRIP
    float2 pos[4] = {
        float2(-1.0f,  1.0f), // top-left
        float2(-1.0f, -1.0f), // bottom-left
        float2( 1.0f,  1.0f), // top-right
        float2( 1.0f, -1.0f)  // bottom-right
    };

    // UVs matching the vertex positions
    float2 tex[4] = {
        float2(0.0f, 0.0f), // top-left
        float2(0.0f, 1.0f), // bottom-left
        float2(1.0f, 0.0f), // top-right
        float2(1.0f, 1.0f)  // bottom-right
    };

    o.pos = float4(pos[vertexID], 0.0f, 1.0f);
    o.uv  = tex[vertexID];
    return o;
}
