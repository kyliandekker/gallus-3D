// -----------------------------------------------------------------------------------------
// ROOT PARAMS
// -----------------------------------------------------------------------------------------
class RootParameter
{
    name = "";
    shaderName = "";
    register = "";
    type = "";
	
	constructor (name, shaderName, register, type)
	{
		this.name = name;
		this.shaderName = shaderName;
		this.register = register;
		this.type = type;
	}
}

// -----------------------------------------------------------
function readLine(line)
{
	if (!line)
	{
		return null;
	}
	
	const splitLine = line.trim().split(/\s+/);

	const name = splitLine[0];
	const shaderName = splitLine[1];
	const register = splitLine[2];
	const type = splitLine[3].replace(";", "");
	
	var rootParameter = new RootParameter(name, shaderName, register, type);
	return rootParameter;
}

export function getRootParameters(lines)
{
    const m = lines[0] == "root_parameters";
	if (m)
	{
		var rootParameters = [];
		for (var i = 1; i < lines.length; i++)
		{
			var rootParameter = readLine(lines[i]);
			if (rootParameter)
			{
				rootParameters.push(rootParameter);
			}
		}
		return rootParameters;
	}
	
	return null;
}