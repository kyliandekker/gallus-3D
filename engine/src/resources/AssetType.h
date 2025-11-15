#pragma once

#include <string>

#ifdef _EDITOR
#include <imgui/imgui.h>
#endif

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
				case AssetType::Prefab:
				{
					return "Prefab";
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

#ifdef _EDITOR
		inline ImVec4 ToImVec4(int r, int g, int b, int a)
		{
			return ImVec4(
				static_cast<float>(r) / 255.0f,
				static_cast<float>(g) / 255.0f,
				static_cast<float>(b) / 255.0f,
				static_cast<float>(a) / 255.0f
			);
		}

		inline ImVec4 AssetTypeToColor(AssetType a_AssetType)
		{
			switch (a_AssetType)
			{
				case AssetType::Scene:
				case AssetType::Prefab:
				{
					return ToImVec4(255, 131, 48, 255);
				}
				case AssetType::Sprite:
				{
					return ToImVec4(38, 127, 0, 255);
				}
				case AssetType::Sound:
				case AssetType::Song:
				case AssetType::VO:
				{
					return ToImVec4(0, 148, 255, 255);
				}
				case AssetType::Animation:
				case AssetType::AnimationGraph:
				{
					return ToImVec4(255, 178, 189, 255);
				}
				case AssetType::PixelShader:
				case AssetType::VertexShader:
				case AssetType::ShaderBind:
				{
					return ToImVec4(170, 0, 0, 255);
				}
				case AssetType::Mesh:
				{
					return ToImVec4(0, 206, 206, 255);
				}
				default:
				{
					return ToImVec4(255, 255, 255, 255);
				}
			}
		}
#endif
	}
}