import { getField } from './field.js';

// -----------------------------------------------------------------------------------------
// BUFFER
// -----------------------------------------------------------------------------------------
class Buffer
{
    name = "";
    register = "";
	fields = [];
	
	// -----------------------------------------------------------
    constructor(name, register)
    {
        this.name = name;
        this.register = register;
    }
	
	// -----------------------------------------------------------
	addField(field)
	{
		this.fields.push(field);
	}
}

// -----------------------------------------------------------
export function getBuffer(lines)
{
    const m = lines[0].match(/cbuffer\s+(\w+).*register\s*=\s*(\w+)/);
	if (m)
	{
        const bufferName = m[1];
        const register = m[2];
		
		var buffer = new Buffer(bufferName, register);

		for (var i = 1; i < lines.length; i++)
		{
			buffer.addField(getField(lines[i]));
		}
		
		return buffer;
	}
	return null;
}