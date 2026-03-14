// -----------------------------------------------------------------------------------------
// FIELD
// -----------------------------------------------------------------------------------------
export class Field
{
    type = "";
    name = "";
    amount = 1;
	
	// -----------------------------------------------------------
    constructor(type, name, amount = 1)
    {
        this.type = type;
        this.name = name;
        this.amount = amount;
    }
}

// -----------------------------------------------------------
export function getField(raw)
{
	const line = raw.trim();
    const m = line.match(/(\w+)\s+(\w+)(?:\[(\d+)\])?;/);

	if (m)
	{
		const type = m[1];
		const name = m[2].replace(";", "");
		const amount = m[3] ? parseInt(m[3]) : 1; // e.g., 64
		
		return new Field(type, name, amount);
	}
	
	return null;
}