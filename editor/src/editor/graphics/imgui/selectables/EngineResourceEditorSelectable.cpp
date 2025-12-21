#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "EngineResourceEditorSelectable.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/imgui/font_icon.h"

// resources
#include "resources/FileResource.h"
#include "resources/AssetType.h"

// editor
#include "editor/core/EditorEngine.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			EngineResourceEditorSelectable::EngineResourceEditorSelectable(ImGuiWindow& a_Window, std::weak_ptr<resources::EngineResource> a_pResource) : EditorSelectable(a_Window), m_pResource(a_pResource)
			{
				m_bShowRename = true;
				m_bShowDelete = true;
				m_bShowShowInExplorer = true;

				SetIcon();
				if (auto pResource = m_pResource.lock())
				{
					SetDisplayName(pResource->GetName());
				}
			}

			void EngineResourceEditorSelectable::SetIcon()
			{
				if (auto pResource = m_pResource.lock())
				{
					m_sIcon = AssetTypeToFileIcon(pResource->GetAssetType());
				}
			}

			std::string EngineResourceEditorSelectable::GetIcon() const
			{
				return m_sIcon;
			}

			void EngineResourceEditorSelectable::SetIcon(const std::string& a_sIcon)
			{
				m_sIcon = a_sIcon;
			}

			void EngineResourceEditorSelectable::RenderList(bool& a_bClicked, bool& a_bRightClicked, bool& a_bDoubleClicked, bool a_bSelected, bool a_bInContextMenu)
			{
				ImVec2 initialPos = ImGui::GetCursorPos();
				ImVec2 buttonStart = ImGui::GetCursorScreenPos();

				ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 32);

				auto pResource = m_pResource.lock();
				if (!pResource)
				{
					return;
				}

				if (ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "FILES_FOLDERS_INVIS_BUTTON_" + pResource->GetName()).c_str(), buttonSize))
				{
					a_bClicked = true;
				}
				a_bDoubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0);
				a_bRightClicked = ImGui::IsItemHovered() && ImGui::IsMouseClicked(1);

				ImVec2 buttonEnd = ImVec2(buttonStart.x + buttonSize.x, buttonStart.y + buttonSize.y);
				if (ImGui::IsItemHovered())
				{
					ImGui::GetWindowDrawList()->AddRectFilled(buttonStart, buttonEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered)));
				}
				if (a_bSelected)
				{
					ImGui::GetWindowDrawList()->AddRectFilled(buttonStart, buttonEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive)));
				}

				ImVec2 iconSize = ImGui::CalcTextSize(m_sIcon.c_str());

				float verticalOffset = (buttonSize.y - iconSize.y) / 2.0f;

				ImVec2 contentStartPos = ImVec2(initialPos.x + m_Window.GetFontSize(), initialPos.y + verticalOffset);
				ImGui::SetCursorPos(contentStartPos);
				ImGui::TextColored(AssetTypeToColor(pResource->GetAssetType()), m_sIcon.c_str());

				ImVec2 textSize = ImGui::CalcTextSize(m_sDisplayName.c_str());

				verticalOffset = (buttonSize.y - textSize.y) / 2.0f;

				ImVec2 textPos = contentStartPos + ImVec2(m_Window.GetFontSize() * 1.5f, 0);
				ImGui::SetCursorPos(textPos);

				ImGui::Text(m_sDisplayName.c_str());

				std::string assetType = "Engine " + gallus::resources::AssetTypeToString(pResource->GetAssetType());

				textSize = ImGui::CalcTextSize(assetType.c_str());

				verticalOffset = (buttonSize.y - textSize.y) / 2.0f;

				textPos = initialPos + ImVec2(350, verticalOffset);
				ImGui::SetCursorPos(textPos);

				ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
				textColor.w = 0.5f;
				ImGui::TextColored(textColor, assetType.c_str());

				ImGui::SetCursorPos(ImVec2(initialPos.x, initialPos.y + buttonSize.y));
			}

			static std::string TruncateTextToFit(const std::string& text, float maxWidth)
			{
				ImFont* font = ImGui::GetFont();
				float textWidth = ImGui::CalcTextSize(text.c_str()).x;

				if (textWidth <= maxWidth)
					return text; // Fits fine, no change

				std::string result = text;
				const std::string ellipsis = "...";
				float ellipsisWidth = ImGui::CalcTextSize(ellipsis.c_str()).x;

				while (!result.empty() && ImGui::CalcTextSize((result + ellipsis).c_str()).x > maxWidth)
					result.pop_back();

				return result + ellipsis;
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE