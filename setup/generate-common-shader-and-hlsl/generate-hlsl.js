import fs from 'fs';
import path from 'path';

import { getBuffer } from './modules/buffer.js';
import { getVSInput } from './modules/vsInput.js';
import { getPSInput } from './modules/psInput.js';
import { getRootParameters } from './modules/rootParameter.js';
import { constructCpp } from './modules/cppFileGenerator.js';
import { constructHlsl } from './modules/hlslFileGenerator.js';

const INPUT_FILE = "./CommonShaderDefs.meta";

// -------------------------------------------------------------------
var currentGeneratedStruct = [];

// -------------------------------------------------------------------

const rootDir = process.argv[2];

console.log(`Using root directory: "${rootDir}".`);
console.log(`Reading file: "${INPUT_FILE}".`);
const LINES = fs.readFileSync(INPUT_FILE, "utf8").split(/\r?\n/);

var BUFFERS = [];
var VSINPUTS = [];
var PSINPUTS = [];
var ROOT_PARAMETERS = [];

for (let raw of LINES)
{
	const line = raw.trim();
	if (!line)
	{
		continue;
	}
	
	if (line == "{")
	{
		continue;
	}
	
	if (line == "};")
	{
		var buffer = getBuffer(currentGeneratedStruct);
		var vsInput = getVSInput(currentGeneratedStruct);
		var psInput = getPSInput(currentGeneratedStruct);
		var rootParameters = getRootParameters(currentGeneratedStruct);
		
		if (buffer)
		{
			BUFFERS.push(buffer);
		}
		if (vsInput)
		{
			VSINPUTS = vsInput;
		}
		if (psInput)
		{
			PSINPUTS = psInput;
		}
		if (rootParameters)
		{
			ROOT_PARAMETERS = rootParameters;
		}
	
		currentGeneratedStruct = [];
	}
	else
	{
		currentGeneratedStruct.push(line);
	}
}

// console.dir(BUFFERS, { depth: null });

const cppFileContent = constructCpp(BUFFERS, VSINPUTS, ROOT_PARAMETERS);
const hlslFileContent = constructHlsl(BUFFERS, VSINPUTS, PSINPUTS, ROOT_PARAMETERS);

const OUT_CPP = rootDir + "/engine/src/graphics/dx12/ShaderDefs.h";
fs.mkdirSync(path.dirname(OUT_CPP), { recursive: true });
fs.writeFileSync(OUT_CPP, cppFileContent.join("\n"));

const OUT_HLSL = rootDir + "/data/assets/shaders/common.hlsl";
fs.mkdirSync(path.dirname(OUT_HLSL), { recursive: true });
fs.writeFileSync(OUT_HLSL, hlslFileContent.join("\n"));