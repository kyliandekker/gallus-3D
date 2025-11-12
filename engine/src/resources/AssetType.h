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
			Sprite,
			Sound,
			Song,
			VO,
			Animation,
			PixelShader,
			VertexShader,
			Prefab,
			ShaderBind,
			Mesh,
			AnimationGraph
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
					return "Scene";
				}
				case AssetType::Sprite:
				{
					return "Sprite";
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
				case AssetType::ShaderBind:
				{
					return "Shader Bind";
				}
				case AssetType::Mesh:
				{
					return "Mesh";
				}
				case AssetType::AnimationGraph:
				{
					return "Animation Graph";
				}
				default:
				{
					return "";
				}
			}
		}
	}
}