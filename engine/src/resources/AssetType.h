#pragma once

// external
#include <string>

#ifdef _EDITOR
#include <imgui/imgui.h>

// graphics
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
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
			None,
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
			Material,
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
	}
#ifdef _EDITOR
	namespace graphics
	{
		namespace imgui
		{
		inline ImVec4 Shade(const ImVec4& color, float factor)
		{
			return ImVec4(
				color.x * factor,
				color.y * factor,
				color.z * factor,
				color.w
			);
		}

		inline ImVec4 ShiftWithinFamily(const ImVec4& base, float slot, float totalSlots)
		{
			float h, s, v;
			ImGui::ColorConvertRGBtoHSV(base.x, base.y, base.z, h, s, v);

			const float maxHueDelta = 0.083f; 

			float offset = ((slot / (totalSlots - 1.0f)) - 0.5f) * 2.0f * maxHueDelta;

			// If saturation is too low → modify value instead
			if (s < 0.25f)
			{
				float vShift = offset * 0.6f;
				v = std::clamp(v + vShift, 0.1f, 1.0f);
			}
			else
			{
				h = fmodf(h + offset, 1.0f);
				if (h < 0.0f)
				{
					h += 1.0f;
				}

				// Prevent neon edge cases
				s = std::clamp(s * (1.0f - fabs(offset) * 0.5f), 0.1f, 1.0f);
			}

			ImVec4 out;
			ImGui::ColorConvertHSVtoRGB(h, s, v, out.x, out.y, out.z);
			out.w = base.w;
			return out;
		}

		inline ImVec4 AssetTypeToColor(resources::AssetType a_AssetType)
		{
			const ImVec4 base = g_vAccentColor;

			switch (a_AssetType)
			{
				case resources::AssetType::Scene:
				case resources::AssetType::Prefab:
				{
					return ShiftWithinFamily(base, 0, 6);
				}
				case resources::AssetType::Sprite:
				{
					return ShiftWithinFamily(base, 1, 6);
				}
				case resources::AssetType::Sound:
				case resources::AssetType::Song:
				case resources::AssetType::VO:
				{
					return ShiftWithinFamily(base, 2, 6);
				}
				case resources::AssetType::Animation:
				{
					return ShiftWithinFamily(base, 3, 6);
				}
				case resources::AssetType::PixelShader:
				case resources::AssetType::VertexShader:
				case resources::AssetType::ShaderBind:
				{
					return ShiftWithinFamily(base, 4, 6);
				}
				case resources::AssetType::Mesh:
				{
					return ShiftWithinFamily(base, 5, 6);
				}
				case resources::AssetType::Material:
				{
					return ShiftWithinFamily(base, 6, 6);
				}
				default:
				{
					return base;
				}
			}
		}

			/// <summary>
			/// Converts an asset type enumeration value to its corresponding file icon.
			/// </summary>
			/// <param name="a_AssetType">The asset type to convert.</param>
			/// <returns>A string containing the file icon.</returns>
			inline std::string AssetTypeToFileIcon(resources::AssetType a_AssetType)
			{
				switch (a_AssetType)
				{
					case resources::AssetType::Folder:
					{
						return font::ICON_FILE_FOLDER;
					}
					case resources::AssetType::Scene:
					{
						return font::ICON_FILE_SCENE;
					}
					case resources::AssetType::Sprite:
					{
						return font::ICON_FILE_IMAGE;
					}
					case resources::AssetType::Sound:
					{
						return font::ICON_FILE_AUDIO;
					}
					case resources::AssetType::Song:
					{
						return font::ICON_FILE_MUSIC;
					}
					case resources::AssetType::VO:
					{
						return font::ICON_FILE_VO;
					}
					case resources::AssetType::Animation:
					{
						return font::ICON_FILE_ANIMATION;
					}
					case resources::AssetType::PixelShader:
					{
						return font::ICON_FILE_SETTINGS;
					}
					case resources::AssetType::VertexShader:
					{
						return font::ICON_FILE_SETTINGS;
					}
					case resources::AssetType::Prefab:
					{
						return font::ICON_FILE_MODEL;
					}
					case resources::AssetType::ShaderBind:
					{
						return font::ICON_FILE_SETTINGS;
					}
					case resources::AssetType::Mesh:
					{
						return font::ICON_FILE_MODEL;
					}
					case resources::AssetType::Material:
					{
						return font::ICON_FILE_MATERIAL;
					}
					default:
					{
						return "";
					}
				}
			}

			/// <summary>
			/// Converts an asset type enumeration value to its corresponding icon.
			/// </summary>
			/// <param name="a_AssetType">The asset type to convert.</param>
			/// <returns>A string containing the icon.</returns>
			//inline std::string AssetTypeToIcon(AssetType a_AssetType)
			//{
			//	switch (a_AssetType)
			//	{
			//		case AssetType::Scene:
			//		{
			//			return font::ICON_GRID;
			//		}
			//		case AssetType::Sprite:
			//		{
			//			return font::ICON_IMAGE;
			//		}
			//		case AssetType::Sound:
			//		{
			//			return font::ICON_AUDIO;
			//		}
			//		case AssetType::Song:
			//		{
			//			return font::ICON_MUSIC;
			//		}
			//		case AssetType::VO:
			//		{
			//			return font::ICON_VO;
			//		}
			//		case AssetType::Animation:
			//		{
			//			return font::ICON_ANIMATION;
			//		}
			//		case AssetType::PixelShader:
			//		{
			//			return font::ICON_SETTINGS;
			//		}
			//		case AssetType::VertexShader:
			//		{
			//			return font::ICON_SETTINGS;
			//		}
			//		case AssetType::Prefab:
			//		{
			//			return font::ICON_MODEL;
			//		}
			//		case AssetType::ShaderBind:
			//		{
			//			return font::ICON_SETTINGS;
			//		}
			//		case AssetType::Mesh:
			//		{
			//			return font::ICON_MODEL;
			//		}
			//		case AssetType::AnimationGraph:
			//		{
			//			return "Animation Graph";
			//		}
			//		default:
			//		{
			//			return "";
			//		}
			//	}
			//}
		}
	}
#endif
}