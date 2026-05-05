import { GENERATION_TYPE, GetGeneratedType } from './definitions.js';

// -------------------------------------------------------------------
export function constructCpp(buffers, vsInput, rootParameters)
{
	var cpp = [];
	
	cpp.push(`#pragma once\n`);
	cpp.push(`#include "./DX12PCH.h"`);
	cpp.push(``);
	cpp.push(`// GENERATED FILE - DO NOT EDIT`);
	cpp.push(``);
	cpp.push(`namespace gallus::graphics::dx12`);
	cpp.push(`{`);
	
	if (buffers)
	{
		cpp.push(`\t// BUFFERS`);
		cpp.push(``);
		
		for (var i = 0; i < buffers.length; i++)
		{
			const buffer = buffers[i];
			cpp.push(`\tstruct ${buffer.name} // registered at ${buffer.register}.`);
			cpp.push(`\t{`);
			
			var totalSize = 0;
			for (var u = 0; u < buffer.fields.length; u++)
			{
				const field = buffer.fields[u];
				if (!field)
				{
					continue;
				}
				
				const type = GetGeneratedType(field.type, GENERATION_TYPE.GENERATION_TYPE_CPP);
				const size = GetGeneratedType(field.type, GENERATION_TYPE.GENERATION_TYPE_SIZE) * field.amount;
				
				var fullname = `${field.name}`;
				if (field.amount > 1)
				{
					fullname += `[${field.amount.toString()}]`;
				}
				cpp.push(`\t\t${type} ${fullname}; // ${size} bytes.`);
				
				totalSize += size;
			}
			
			const missingAlignment = totalSize % 16;
			if (missingAlignment != 0)
			{
				cpp.push(`\t// TODO: Alignment is not correct for GPU. Fix!!! (${totalSize} bytes total, missing ${16 - missingAlignment} bytes)`);
			}
			else
			{
				cpp.push(`\t}; // total size = ${totalSize}.`);
			}
			
			if (i < buffers.length - 1)
			{
				cpp.push(``);
			}
		}
		cpp.push(``);
	}
	
	if (vsInput)
	{
		cpp.push(`\t// INPUT`);
		cpp.push(``);
		
		cpp.push(`\tconst D3D12_INPUT_ELEMENT_DESC g_aInputLayout[] = {`);
		
		for (var i = 0; i < vsInput.fields.length; i++)
		{
			const field = vsInput.fields[i];
			if (!field)
			{
				continue;
			}
			console.log(field);
			const type = GetGeneratedType(field.type, GENERATION_TYPE.GENERATION_TYPE_DXGI);
			
			cpp.push(`\t\t{ "${field.name}", 0, ${type}, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },`);
		}
		
		cpp.push(`\t};`);
		cpp.push(``);
	}
	
	if (rootParameters)
	{
		cpp.push(`\t// ROOT PARAMETERS`);
		cpp.push(``);
		
		cpp.push(`\tenum RootParameters`);
		cpp.push(`\t{`);
		
		for (var i = 0; i < rootParameters.length; i++)
		{
			const rootParameter = rootParameters[i];
			cpp.push(`\t\t${rootParameter.name} = ${i}, // registered at ${rootParameter.register}.`);
		}
		
		cpp.push(`\t\tNumRootParameters = ${rootParameters.length + 1},`);
		cpp.push(`\t};`);
	}
	
	cpp.push(`}`);
	
	return cpp;
}