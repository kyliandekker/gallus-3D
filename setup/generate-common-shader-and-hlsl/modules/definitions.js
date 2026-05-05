export const GENERATION_TYPE =
{
    GENERATION_TYPE_NONE: 0,
    GENERATION_TYPE_CPP: 1,
    GENERATION_TYPE_HLSL: 2,
    GENERATION_TYPE_DXGI: 3,
    GENERATION_TYPE_SIZE: 4
};

// -----------------------------------------------------------
const GENERATION_MAP = new Map([
    ["float",
    {
        [GENERATION_TYPE.GENERATION_TYPE_CPP]: "float",
        [GENERATION_TYPE.GENERATION_TYPE_HLSL]: "float",
        [GENERATION_TYPE.GENERATION_TYPE_SIZE]: 4,
        [GENERATION_TYPE.GENERATION_TYPE_DXGI]: "DXGI_FORMAT_R32_FLOAT"
    }],

    ["float2",
    {
        [GENERATION_TYPE.GENERATION_TYPE_CPP]: "DirectX::XMFLOAT2",
        [GENERATION_TYPE.GENERATION_TYPE_HLSL]: "float2",
        [GENERATION_TYPE.GENERATION_TYPE_SIZE]: 8,
        [GENERATION_TYPE.GENERATION_TYPE_DXGI]: "DXGI_FORMAT_R32G32_FLOAT"
    }],

    ["float3",
    {
        [GENERATION_TYPE.GENERATION_TYPE_CPP]: "DirectX::XMFLOAT3",
        [GENERATION_TYPE.GENERATION_TYPE_HLSL]: "float3",
        [GENERATION_TYPE.GENERATION_TYPE_SIZE]: 12,
        [GENERATION_TYPE.GENERATION_TYPE_DXGI]: "DXGI_FORMAT_R32G32B32_FLOAT"
    }],

    ["float4",
    {
        [GENERATION_TYPE.GENERATION_TYPE_CPP]: "DirectX::XMFLOAT4",
        [GENERATION_TYPE.GENERATION_TYPE_HLSL]: "float4",
        [GENERATION_TYPE.GENERATION_TYPE_SIZE]: 16,
        [GENERATION_TYPE.GENERATION_TYPE_DXGI]: "DXGI_FORMAT_R32G32B32A32_FLOAT"
    }],

    ["uint4",
    {
        [GENERATION_TYPE.GENERATION_TYPE_CPP]: "DirectX::XMUINT4",
        [GENERATION_TYPE.GENERATION_TYPE_HLSL]: "uint4",
        [GENERATION_TYPE.GENERATION_TYPE_SIZE]: 16,
        [GENERATION_TYPE.GENERATION_TYPE_DXGI]: "DXGI_FORMAT_R32G32B32A32_UINT"
    }],

    ["uint",
    {
        [GENERATION_TYPE.GENERATION_TYPE_CPP]: "uint32_t",
        [GENERATION_TYPE.GENERATION_TYPE_HLSL]: "uint",
        [GENERATION_TYPE.GENERATION_TYPE_SIZE]: 4,
        [GENERATION_TYPE.GENERATION_TYPE_DXGI]: "DXGI_FORMAT_R32G32B32A32_UINT"
    }],

    ["float4x4",
    {
        [GENERATION_TYPE.GENERATION_TYPE_CPP]: "DirectX::XMMATRIX",
        [GENERATION_TYPE.GENERATION_TYPE_HLSL]: "float4x4",
        [GENERATION_TYPE.GENERATION_TYPE_SIZE]: 64,
        [GENERATION_TYPE.GENERATION_TYPE_DXGI]: ""
    }]
]);

// -----------------------------------------------------------
export function GetGeneratedType(type, generationType)
{
    const entry = GENERATION_MAP.get(type);
    if (!entry)
    {
        throw new Error(`Unknown shader type: "${type}"`);
    }

    return entry[generationType];
}