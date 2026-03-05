dxc PixelShader.hlsl ^
    -E main ^
    -T ps_6_0 ^
    -Fo PixelShader.cso
	
dxc VertexShader.hlsl ^
    -E main ^
    -T vs_6_0 ^
    -Fo VertexShader.cso
	
dxc pixelShaderUvDebug.hlsl ^
    -E main ^
    -T ps_6_0 ^
    -Fo PixelShaderUvDebug.cso
	
dxc renderTexPixelShader.hlsl ^
    -E main ^
    -T ps_6_0 ^
    -Fo renderTexPixelShader.cso
	
dxc renderTexVertexShader.hlsl ^
    -E main ^
    -T vs_6_0 ^
    -Fo renderTexVertexShader.cso