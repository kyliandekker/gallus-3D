#include "Common.hlsl"

PSInput main(uint vertexID : SV_VertexID)
{
    PSInput output;
    
    // Generate a full-screen triangle using vertex ID
    // This creates a triangle that extends beyond screen bounds to ensure full coverage
    // Vertex 0: (-1, -1) -> UV (0, 1)
    // Vertex 1: (-1,  3) -> UV (0, -1) 
    // Vertex 2: ( 3, -1) -> UV (2, 1)
    
    output.pos.x = (float)(vertexID / 2) * 4.0 - 1.0;
    output.pos.y = (float)(vertexID % 2) * 4.0 - 1.0;
    output.pos.z = 0.0;
    output.pos.w = 1.0;

    output.normal = float3(0.0f, 0.0f, 1.0f);
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Generate UV coordinates (0,0) to (1,1) for the visible screen area
    output.uv.x = (float)(vertexID / 2) * 2.0;
    output.uv.y = 1.0 - (float)(vertexID % 2) * 2.0;
    
    return output;
}