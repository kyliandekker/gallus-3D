const fs = require("fs");
const path = require("path");

const INPUT_FILE = "./CommonShaderDefs.meta";
const OUT_CPP    = "../../engine/src/graphics/dx12/ShaderDefs.h";
const OUT_HLSL   = "../../data/assets/shaders/common.hlsl";

const TYPE_MAP_CPP = {
    float: "float",
    float2: "DirectX::XMFLOAT2",
    float3: "DirectX::XMFLOAT3",
    float4: "DirectX::XMFLOAT4",
    float4x4: "DirectX::XMMATRIX"
};

const TYPE_MAP_HLSL = {
    float: "float",
    float2: "float2",
    float3: "float3",
    float4: "float4",
    float4x4: "float4x4"
};

const TYPE_SIZE = {
    float: 4,
    float2: 8,
    float3: 12,
    float4: 16,
    float4x4: 64
};

// Map from HLSL type to DXGI_FORMAT for input layout
const DXGI_TYPE_MAP = {
    float: "DXGI_FORMAT_R32_FLOAT",
    float2: "DXGI_FORMAT_R32G32_FLOAT",
    float3: "DXGI_FORMAT_R32G32B32_FLOAT",
    float4: "DXGI_FORMAT_R32G32B32A32_FLOAT"
};

function align16(v) { return Math.ceil(v / 16) * 16; }

const ir = { cbuffers: {}, vertexInputs: [], rootParams: [] };
const lines = fs.readFileSync(INPUT_FILE, "utf8").split(/\r?\n/);

let currentCBuffer = null;
let currentVS = null;
let skipNextLine = false;

for (let raw of lines) {
    const line = raw.trim();
    if (!line) continue;

    // Parse cbuffer annotation
    let m = line.match(/@shader_cbuffer\s*\(\s*name\s*=\s*(\w+)\s*,\s*register\s*=\s*(\w+)\s*\)/);
    if (m) {
        const name = m[1];
        const reg = m[2];
        currentCBuffer = { name, reg, fields: [] };
        ir.cbuffers[name] = currentCBuffer;
        currentVS = null;
        skipNextLine = true;
        continue;
    }

    // Vertex input annotation
    if (line.startsWith("// @shader_vertex_input")) {
        currentVS = { name: "VSInput", fields: [] };
        ir.vertexInputs.push(currentVS);
        currentCBuffer = null;
        skipNextLine = true;
        continue;
    }

    // Root parameter annotation
    if (line.startsWith("// @shader_root_parameters")) {
        currentCBuffer = null;
        currentVS = null;
        continue;
    }

    if (skipNextLine) {
        skipNextLine = false;
        continue;
    }

    // Inside cbuffer
    if (currentCBuffer) {
        m = line.match(/^(\w+)\s+(\w+);?$/);
        if (m) currentCBuffer.fields.push({ type: m[1], name: m[2] });
        continue;
    }

    // Inside vertex input
    if (currentVS) {
        const cleanLine = line.replace(/;$/, "");
        m = cleanLine.match(/^(\w+)\s+(\w+)\s+(\w+)$/);
        if (m) currentVS.fields.push({ semantic: m[1], type: m[2], name: m[3] });
        continue;
    }

    // Root parameter
    m = line.match(/^(\w+)\s+(\w+)\s+(\w+)\s+(\w+);?$/);
    if (m) ir.rootParams.push({ name: m[1], reg: m[2], kind: m[3], target: m[4] });
}

// ---------------- EMIT C++ ----------------

let cpp = [];
cpp.push("#pragma once");
cpp.push("#include \"./DX12PCH.h\"");
cpp.push("");
cpp.push("// GENERATED FILE - DO NOT EDIT\n");

for (const cb of Object.values(ir.cbuffers)) {
    cpp.push(`struct ${cb.name}`);
    cpp.push("{");
    let offset = 0;
    for (const f of cb.fields) {
        cpp.push(`    ${TYPE_MAP_CPP[f.type]} ${f.name};`);
        offset += TYPE_SIZE[f.type];
    }
    if (offset % 16 !== 0) {
        const pad = (align16(offset) - offset) / 4;
        cpp.push(`    float _Padding[${pad}];`);
    }
    cpp.push("};\n");
}

// Vertex input layout
if (ir.vertexInputs.length > 0) {
    const vs = ir.vertexInputs[0];
    cpp.push("const D3D12_INPUT_ELEMENT_DESC g_aInputLayout[] = {");
    vs.fields.forEach(f => {
		console.log(f);
        cpp.push(`    { "${f.name}", 0, ${DXGI_TYPE_MAP[f.type]}, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },`);
    });
    cpp.push("};\n");
}

// Root parameters enum
cpp.push("enum RootParameters");
cpp.push("{");
ir.rootParams.forEach((p, i) => cpp.push(`    ${p.name} = ${i},`));
cpp.push("    NumRootParameters");
cpp.push("};\n");

fs.mkdirSync(path.dirname(OUT_CPP), { recursive: true });
fs.writeFileSync(OUT_CPP, cpp.join("\n"));

// ---------------- EMIT HLSL ----------------

let hlsl = [];
hlsl.push("// GENERATED FILE - DO NOT EDIT\n");

// CBuffers
for (const cb of Object.values(ir.cbuffers)) {
    hlsl.push(`cbuffer ${cb.name} : register(${cb.reg})`);
    hlsl.push("{");
    let offset = 0;
    let padIndex = 0;
    for (const f of cb.fields) {
        const size = TYPE_SIZE[f.type];
        if ((offset % 16) + size > 16) offset = align16(offset);
        hlsl.push(`    ${TYPE_MAP_HLSL[f.type]} ${f.name};`);
        offset += size;
    }
    if (offset % 16 !== 0) {
        const pad = (align16(offset) - offset) / 4;
        hlsl.push(`    float${pad > 1 ? pad : ""} _Padding${padIndex};`);
    }
    hlsl.push("};\n");
}

// Vertex input + PSInput
for (const vs of ir.vertexInputs) {
    // VSInput
    hlsl.push(`struct ${vs.name}`);
    hlsl.push("{");
    for (const f of vs.fields) {
        hlsl.push(`    ${TYPE_MAP_HLSL[f.type]} ${f.name} : ${f.semantic};`);
    }
    hlsl.push("};\n");

    // PSInput
    hlsl.push("struct PSInput");
    hlsl.push("{");
    for (const f of vs.fields) {
        if (f.semantic === "POSITION") {
            hlsl.push(`    float4 ${f.name} : SV_POSITION;`);
        } else {
            hlsl.push(`    ${TYPE_MAP_HLSL[f.type]} ${f.name} : ${f.semantic};`);
        }
    }
    hlsl.push("};\n");
}

// Root parameters (any type except cbuffer)
for (const r of ir.rootParams) {
    if (r.kind.toLowerCase() === "cbuffer") continue; // skip duplicates
    hlsl.push(`${r.kind} ${r.target} : register(${r.reg});`);
}

fs.mkdirSync(path.dirname(OUT_HLSL), { recursive: true });
fs.writeFileSync(OUT_HLSL, hlsl.join("\n"));

console.log("Generated:");
console.log(" ", OUT_CPP);
console.log(" ", OUT_HLSL);
