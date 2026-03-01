#include "ListItemEditorSelectable.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "imgui_system/ImGuiSystem.h"
#include "imgui_system/font_icon.h"

// resources
#include "resources/AssetType.h"

namespace gallus::graphics::imgui
{
	std::string AssetTypeToFileIcon(resources::AssetType a_AssetType)
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
			case resources::AssetType::Texture:
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

	//---------------------------------------------------------------------
	ImVec4 Shade(const ImVec4& color, float factor)
	{
		return ImVec4(
			color.x * factor,
			color.y * factor,
			color.z * factor,
			color.w
		);
	}

	//---------------------------------------------------------------------
	ImVec4 ShiftWithinFamily(const ImVec4& base, float slot, float totalSlots)
	{
		float h, s, v;
		ImGui::ColorConvertRGBtoHSV(base.x, base.y, base.z, h, s, v);

		const float maxHueDelta = 0.083f;

		float offset = ((slot / (totalSlots - 1.0f)) - 0.5f) * 2.0f * maxHueDelta;

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

	//---------------------------------------------------------------------
	ImVec4 AssetTypeToColor(resources::AssetType a_AssetType)
	{
		const ImVec4 base = GetAccentColor();

		switch (a_AssetType)
		{
			case resources::AssetType::Scene:
			case resources::AssetType::Prefab:
			{
				return ShiftWithinFamily(base, 0, 6);
			}
			case resources::AssetType::Texture:
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

	//---------------------------------------------------------------------
	ListItemEditorSelectable::ListItemEditorSelectable(ImGuiSystem& a_System) : EditorSelectable(a_System)
	{}

	//---------------------------------------------------------------------
	void ListItemEditorSelectable::SetIcon(const std::string& a_sIcon)
	{
		m_sIcon = a_sIcon;
	}

	//---------------------------------------------------------------------
	void ListItemEditorSelectable::RenderList(bool a_bSelected)
	{
		ImVec2 assetTypeIconSize = ImGui::CalcTextSize(m_sIcon.c_str());

		ImGui::TextColored(a_bSelected ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : AssetTypeToColor(m_AssetType), m_sIcon.c_str());
		ImGui::TableNextColumn();
		ImGui::Text(m_sName.c_str());
		ImGui::TableNextColumn();
		ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		textColor.w = 0.5f;
		ImGui::TextColored(textColor, resources::AssetTypeToString(m_AssetType).c_str());
	}

	//---------------------------------------------------------------------
	const std::string& ListItemEditorSelectable::GetName() const
	{
		return m_sName;
	}

	//---------------------------------------------------------------------
	const std::string& ListItemEditorSelectable::GetIcon() const
	{
		return m_sIcon;
	}
}