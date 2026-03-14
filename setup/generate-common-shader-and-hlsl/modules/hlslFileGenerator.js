import { GENERATION_TYPE, GetGeneratedType } from './definitions.js';

// -------------------------------------------------------------------
export function constructHlsl(buffers, vsInput, psInput, rootParameters)
{
	var hlsl = [];
	
	hlsl.push(`// GENERATED FILE - DO NOT EDIT`);
	hlsl.push(``);

	if (buffers)
	{
		hlsl.push(`// BUFFERS`);
		hlsl.push(``);
		
		for (var i = 0; i < buffers.length; i++)
		{
			const buffer = buffers[i];
			
			hlsl.push(`cbuffer ${buffer.name} : register(${buffer.register})`);
			hlsl.push(`{`);
			
			var totalSize = 0;
			for (var u = 0; u < buffer.fields.length; u++)
			{
				const field = buffer.fields[u];
				if (!field)
				{
					continue;
				}
				
				const type = GetGeneratedType(field.type, GENERATION_TYPE.GENERATION_TYPE_HLSL);
				const size = GetGeneratedType(field.type, GENERATION_TYPE.GENERATION_TYPE_SIZE) * field.amount;
				
				var fullname = `${field.name}`;
				if (field.amount > 1)
				{
					fullname += `[${field.amount.toString()}]`;
				}
				hlsl.push(`\t${type} ${fullname}; // ${size} bytes.`);
				
				totalSize += size;
			}
			
			const missingAlignment = totalSize % 16;
			if (missingAlignment != 0)
			{
				hlsl.push(`// TODO: Alignment is not correct for GPU. Fix!!! (${totalSize} bytes total, missing ${16 - missingAlignment} bytes)`);
			}
			else
			{
				hlsl.push(`}; // total size = ${totalSize}.`);
			}
				
			if (i < buffers.length - 1)
			{
				hlsl.push(``);
			}
		}
	
		hlsl.push(``);
	}
	
	if (vsInput)
	{
		hlsl.push(`// VSINPUT`);
		hlsl.push(``);
		
		hlsl.push(`struct ${vsInput.name}`);
		hlsl.push(`{`);
		
		var totalSize = 0;
		for (var u = 0; u < vsInput.fields.length; u++)
		{
			const field = vsInput.fields[u];
			if (!field)
			{
				continue;
			}
			
			const type = GetGeneratedType(field.type, GENERATION_TYPE.GENERATION_TYPE_HLSL);
			const size = GetGeneratedType(field.type, GENERATION_TYPE.GENERATION_TYPE_SIZE) * field.amount;
			
			var fullname = `${field.name}`;
			if (field.amount > 1)
			{
				fullname += `[${field.amount.toString()}]`;
			}
			hlsl.push(`\t${type} ${fullname} : ${field.semantic}; // ${size} bytes.`);
			
			totalSize += size;
		}
		
		hlsl.push(`}; // total size = ${totalSize}.`);
		
		const missingAlignment = totalSize % 16;
		if (missingAlignment != 0)
		{
			hlsl.push(`// TODO: Alignment is not correct for GPU. Fix!!! (${totalSize} bytes total, missing ${16 - missingAlignment} bytes)`);
		}
		
		hlsl.push(``);
	}
	
	if (psInput)
	{
		hlsl.push(`// PSINPUT`);
		hlsl.push(``);
		
		hlsl.push(`struct ${psInput.name}`);
		hlsl.push(`{`);
		
		var totalSize = 0;
		for (var u = 0; u < psInput.fields.length; u++)
		{
			const field = psInput.fields[u];
			if (!field)
			{
				continue;
			}
			
			const type = GetGeneratedType(field.type, GENERATION_TYPE.GENERATION_TYPE_HLSL);
			const size = GetGeneratedType(field.type, GENERATION_TYPE.GENERATION_TYPE_SIZE) * field.amount;
			
			var fullname = `${field.name}`;
			if (field.amount > 1)
			{
				fullname += `[${field.amount.toString()}]`;
			}
			hlsl.push(`\t${type} ${fullname} : ${field.semantic}; // ${size} bytes.`);
			
			totalSize += size;
		}
		
		hlsl.push(`}; // total size = ${totalSize}.`);
		
		const missingAlignment = totalSize % 16;
		if (missingAlignment != 0)
		{
			hlsl.push(`// TODO: Alignment is not correct for GPU. Fix!!! (${totalSize} bytes total, missing ${16 - missingAlignment} bytes)`);
		}
		
		hlsl.push(``);
	}
	
	if (rootParameters)
	{
		for (var i = 0; i < rootParameters.length; i++)
		{
			const rootParameter = rootParameters[i];
			if (rootParameter.register[0] == 'b') // ignore cbuffers because we only care about registers that are not buffers.
			{
				continue;
			}
			
			hlsl.push(`${rootParameter.type} ${rootParameter.shaderName} : register(${rootParameter.register});`);
		}
	}
	
	return hlsl;
}