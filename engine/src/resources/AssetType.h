#pragma once

#include <string>

namespace gallus
{
	namespace resources
	{
		/// <summary>
		/// Represents the various types of assets used within the engine.
		/// </summary>
		enum class AssetType
		{
			Folder,
			Scene,
			Texture,
			Sound,
			Song,
			VO,
			Animation,
			PixelShader,
			VertexShader,
		};

		/// <summary>
		/// Converts an asset type enumeration value to its corresponding string representation.
		/// </summary>
		/// <param name="a_AssetType">The asset type to convert.</param>
		/// <returns>A string representing the specified asset type.</returns>
		inline std::string AssetTypeToString(AssetType a_AssetType)
		{
			switch (a_AssetType)
			{
				case AssetType::Scene:
				{
					return "Scene File";
				}
				case AssetType::Texture:
				{
					return "Texture File";
				}
				case AssetType::Sound:
				{
					return "Sound File";
				}
				case AssetType::Song:
				{
					return "Song File";
				}
				case AssetType::VO:
				{
					return "VO File";
				}
				case AssetType::Animation:
				{
					return "Animation File";
				}
				case AssetType::PixelShader:
				{
					return "Pixel Shader File";
				}
				case AssetType::VertexShader:
				{
					return "Vertex Shader File";
				}
				default:
				{
					return "";
				}
			}
		}
	}
}