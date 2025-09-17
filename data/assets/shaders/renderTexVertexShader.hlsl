struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

VertexOutput main(uint vertexID : SV_VertexID)
{
    VertexOutput output;
    
    // Generate a full-screen triangle using vertex ID
    // This creates a triangle that extends beyond screen bounds to ensure full coverage
    // Vertex 0: (-1, -1) -> UV (0, 1)
    // Vertex 1: (-1,  3) -> UV (0, -1) 
    // Vertex 2: ( 3, -1) -> UV (2, 1)
    
    output.position.x = (float)(vertexID / 2) * 4.0 - 1.0;
    output.position.y = (float)(vertexID % 2) * 4.0 - 1.0;
    output.position.z = 0.0;
    output.position.w = 1.0;
    
    // Generate UV coordinates (0,0) to (1,1) for the visible screen area
    output.uv.x = (float)(vertexID / 2) * 2.0;
    output.uv.y = 1.0 - (float)(vertexID % 2) * 2.0;
    
    return output;
}