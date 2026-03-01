#pragma once

// external
#ifndef _RELEASE
#include <string>
#endif

namespace gallus::resources
{
	/// <summary>
	/// Represents the various types of assets used within the engine.
	/// </summary>
	enum class AssetType
	{
		None,
		Folder,
		Scene,
		Texture,
		Sound,
		Song,
		VO,
		Animation,
		PixelShader,
		VertexShader,
		Prefab,
		Mesh,
		Material,
	};
	
#ifndef _RELEASE
	/// <summary>
	/// Converts an asset type enumeration value to its corresponding string representation.
	/// </summary>
	/// <param name="a_AssetType">The asset type to convert.</param>
	/// <returns>A string representing the specified asset type.</returns>
	inline std::string AssetTypeToString(AssetType a_AssetType)
	{
		switch (a_AssetType)
		{
			case AssetType::None:
			{
				return "None";
			}
			case AssetType::Folder:
			{
				return "Folder";
			}
			case AssetType::Scene:
			{
				return "Scene";
			}
			case AssetType::Texture:
			{
				return "Texture";
			}
			case AssetType::Sound:
			{
				return "Sound";
			}
			case AssetType::Song:
			{
				return "Song";
			}
			case AssetType::VO:
			{
				return "VO File";
			}
			case AssetType::Animation:
			{
				return "Animation";
			}
			case AssetType::PixelShader:
			{
				return "Pixel Shader";
			}
			case AssetType::VertexShader:
			{
				return "Vertex Shader";
			}
			case AssetType::Prefab:
			{
				return "Prefab";
			}
			case AssetType::Mesh:
			{
				return "Mesh";
			}
			case AssetType::Material:
			{
				return "Material";
			}
			default:
			{
				return "";
			}
		}
	}
#endif
}