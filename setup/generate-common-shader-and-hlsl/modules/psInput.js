// -----------------------------------------------------------------------------------------
// PIXEL INPUT
// -----------------------------------------------------------------------------------------
class PSInput
{
    name = "";
	fields = [];
	
    constructor(name)
    {
        this.name = name;
    }
	
	addField(field)
	{
		this.fields.push(field);
	}
}

// -----------------------------------------------------------------------------------------
export class ShaderField
{
    type = "";
    name = "";
    semantic = "";
	amount = 1;
	
	// -----------------------------------------------------------
    constructor(type, name, semantic)
    {
        this.type = type;
        this.name = name;
        this.semantic = semantic;
    }
}

// -----------------------------------------------------------
export function getField(raw)
{
	const line = raw.trim();
    const m = line.match(/(\w+)\s+(\w+)\s+(\w+)(?:\[(\d+)\])?;/);

	if (m)
	{
		const type = m[1];
		const name = m[2];
		const semantic = m[3].replace(";", "");
		
		return new ShaderField(type, name, semantic);
	}
	
	return null;
}

// -----------------------------------------------------------------------------------------
export function getPSInput(lines)
{
    const m = lines[0].match(/pixel_input\s+(\w+)/);
	if (m)
	{
        const vsName = m[1];
		
		var psInput = new PSInput(vsName);

		for (var i = 1; i < lines.length; i++)
		{
			psInput.addField(getField(lines[i]));
		}
		
		return psInput;
	}
	
	return null;
}