#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerFileUIView.h"

#include <imgui/imgui_helpers.h>

#include "editor/FileResource.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/imgui/font_icon.h"
#include "resources/AssetType.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			const std::vector<std::string> RESOURCE_ICONS =
			{
				font::ICON_FOLDER,
				font::ICON_FILE_SCENE,
				font::ICON_FILE_IMAGE,
				font::ICON_FILE_AUDIO,
				font::ICON_FILE_MUSIC,
				font::ICON_FILE_VO,
				font::ICON_FILE, // TODO: FIND ICON ANIMATION
				font::ICON_FILE_SETTINGS, // TODO: FIND ICON SHADER
				font::ICON_FILE_SETTINGS, // TODO: FIND ICON SHADER
				font::ICON_FILE_MODEL,
			};

			ExplorerFileUIView::ExplorerFileUIView(ImGuiWindow& a_Window, gallus::editor::FileResource& a_FileResource, ExplorerFileUIView* a_pParent, bool a_bGetChildren) : ImGuiUIView(a_Window),
				m_FileResource(a_FileResource),
				m_pParent(a_pParent)
			{
				SetIcon();
				SetDisplayName(m_FileResource.GetPath().filename().generic_string());

				if (a_bGetChildren)
				{
					for (gallus::editor::FileResource& child : a_FileResource.GetChildren())
					{
						m_aChildren.emplace_back(a_Window, child, this);
					}
				}
			}

			void ExplorerFileUIView::SetIcon()
			{
				if (!m_FileResource.GetMetaData())
				{
					return;
				}
				m_sIcon = RESOURCE_ICONS[(int) m_FileResource.GetMetaData()->GetAssetType()];
			}

			const std::string& ExplorerFileUIView::GetIcon() const
			{
				return m_sIcon;
			}

			void ExplorerFileUIView::SetIcon(const std::string& a_sIcon)
			{
				m_sIcon = a_sIcon;
			}

			bool ExplorerFileUIView::IsFoldedOut() const
			{
				return m_bIsFoldedOut;
			}

			void ExplorerFileUIView::SetFoldedOut(bool a_bIsFoldedOut)
			{
				m_bIsFoldedOut = a_bIsFoldedOut;
			}

			void ExplorerFileUIView::RenderTree(bool& a_bClicked, bool& a_bRightClicked, bool a_bSelected, int a_iIndent, const ImVec2& a_vInitialPos, bool a_bInContextMenu)
			{
				ImGui::SetCursorPos(ImVec2(a_vInitialPos.x, ImGui::GetCursorPos().y));
				ImVec2 buttonStart = ImGui::GetCursorScreenPos();

				ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 32);

				if (ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "FILES_FOLDERS_INVIS_BUTTON_" + m_FileResource.GetPath().generic_string()).c_str(), buttonSize))
				{
					a_bClicked = true;
				}
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

				ImGui::PushFont(m_Window.GetIconFont());
				std::string	folderIcon = m_bIsFoldedOut ? font::ICON_FOLDER_OPEN : font::ICON_FOLDER;
				ImVec2 iconSize = ImGui::CalcTextSize(folderIcon.c_str());

				float verticalOffset = (buttonSize.y - iconSize.y) / 2.0f;

				ImVec2 contentStartPos = ImVec2(a_vInitialPos.x + (ImGui::GetStyle().IndentSpacing * a_iIndent) + (m_Window.GetFontSize() * 2.0f), a_vInitialPos.y + verticalOffset);
				ImGui::SetCursorPos(contentStartPos);
				ImGui::Text(folderIcon.c_str());

				if (HasFolders())
				{
					std::string foldIcon = m_bIsFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN;

					ImVec2 foldIconSize = ImGui::CalcTextSize(foldIcon.c_str());

					verticalOffset = (buttonSize.y - foldIconSize.y) / 2.0f;

					ImGui::SetCursorPos(contentStartPos - ImVec2(iconSize.x, 0));

					ImVec2 foldButtonStart = ImGui::GetCursorScreenPos();
					ImGui::Text(foldIcon.c_str());

					ImVec2 mousePos = ImGui::GetMousePos();
					bool isFoldButtonHovered = (mousePos.x >= foldButtonStart.x && mousePos.x <= foldButtonStart.x + foldIconSize.x &&
						mousePos.y >= foldButtonStart.y && mousePos.y <= foldButtonStart.y + foldIconSize.y);

					bool isFoldButtonClicked = ImGui::IsMouseClicked(0) && isFoldButtonHovered;
					if (isFoldButtonClicked)
					{
						m_bIsFoldedOut = !m_bIsFoldedOut;
					}
				}
				ImGui::PopFont();

				ImVec2 textSize = ImGui::CalcTextSize(m_sDisplayName.c_str());

				verticalOffset = (buttonSize.y - textSize.y) / 2.0f;

				ImGui::SetCursorPos(contentStartPos + ImVec2(m_Window.GetFontSize() * 1.5f, 0));

				ImGui::Text(m_sDisplayName.c_str());

				ImGui::SetCursorPos(ImVec2(a_vInitialPos.x, a_vInitialPos.y + buttonSize.y));
			}

			void ExplorerFileUIView::RenderList(bool& a_bClicked, bool& a_bRightClicked, bool& a_bDoubleClicked, bool a_bSelected, bool a_bInContextMenu)
			{
				ImVec2 initialPos = ImGui::GetCursorPos();
				ImVec2 buttonStart = ImGui::GetCursorScreenPos();

				ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 32);

				if (ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "FILES_FOLDERS_INVIS_BUTTON_" + m_FileResource.GetPath().generic_string()).c_str(), buttonSize))
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

				ImGui::PushFont(m_Window.GetIconFont());
				ImVec2 iconSize = ImGui::CalcTextSize(m_sIcon.c_str());

				float verticalOffset = (buttonSize.y - iconSize.y) / 2.0f;

				ImVec2 contentStartPos = ImVec2(initialPos.x + m_Window.GetFontSize(), initialPos.y + verticalOffset);
				ImGui::SetCursorPos(contentStartPos);
				ImGui::Text(m_sIcon.c_str());

				ImGui::PopFont();

				ImVec2 textSize = ImGui::CalcTextSize(m_sDisplayName.c_str());

				verticalOffset = (buttonSize.y - textSize.y) / 2.0f;

				ImVec2 textPos = contentStartPos + ImVec2(m_Window.GetFontSize() * 1.5f, 0);
				ImGui::SetCursorPos(textPos);

				ImGui::Text(m_sDisplayName.c_str());

				std::string assetType = gallus::resources::AssetTypeToString(m_FileResource.GetMetaData()->GetAssetType());

				textSize = ImGui::CalcTextSize(assetType.c_str());

				verticalOffset = (buttonSize.y - textSize.y) / 2.0f;

				textPos = initialPos + ImVec2(300, verticalOffset);
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

			void ExplorerFileUIView::RenderGrid(const ImVec2& m_vSize, bool& a_bClicked, bool& a_bRightClicked, bool& a_bDoubleClicked, bool a_bSelected, bool a_bInContextMenu)
			{
				ImVec2 initialPos = ImGui::GetCursorPos();
				ImVec2 initialScreenPos = ImGui::GetCursorScreenPos();

				std::string path = m_FileResource.GetPath().generic_string();
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "FILES_INNER_EXPLORER_GRID_" + path).c_str(),
					m_vSize,
					false, // Borderless child
					ImGuiWindowFlags_None
					))
				{
					std::string name = m_FileResource.GetPath().filename().generic_string();
					if (ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "FILES_INNER_EXPLORER_GRID_INVIS_BUTTON_" + path).c_str(), m_vSize))
					{
						a_bClicked = true;
					}
					a_bDoubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0);
					a_bRightClicked = ImGui::IsItemHovered() && ImGui::IsMouseClicked(1);
					if (ImGui::IsItemHovered() || a_bInContextMenu)
					{
						ImGui::GetWindowDrawList()->AddRectFilled(initialScreenPos, initialScreenPos + m_vSize, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered)));
					}
					if (a_bSelected)
					{
						ImGui::GetWindowDrawList()->AddRectFilled(initialScreenPos, initialScreenPos + m_vSize, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive)));
					}

					ImGui::PushFont(m_Window.GetBigIconFont());

					ImVec2 iconSize = ImGui::CalcTextSize(m_sIcon.c_str()); // Replace with your icon size

					// Calculate position to center the icon
					float horizontalOffset = (m_vSize.x - iconSize.x) / 2.0f;
					ImVec2 iconPos = ImVec2(initialScreenPos.x + horizontalOffset, initialScreenPos.y + m_Window.GetFontSize());
					ImGui::SetCursorScreenPos(iconPos);
					ImGui::TextUnformatted(m_sIcon.c_str());
					ImGui::PopFont();

					// Draw file name under icon
					std::string displayedName = TruncateTextToFit(name, m_vSize.x - 4.0f); // little padding

					ImVec2 textSize = ImGui::CalcTextSize(displayedName.c_str());
					float textX = initialScreenPos.x + (m_vSize.x - textSize.x) / 2.0f;
					float textY = iconPos.y + iconSize.y + 4.0f; // gap below icon

					ImGui::SetCursorScreenPos(ImVec2(textX, textY));
					ImGui::TextUnformatted(displayedName.c_str());
				}
				ImGui::EndChild();
			}

			bool ExplorerFileUIView::HasFolders() const
			{
				for (const gallus::editor::FileResource& resource : m_FileResource.GetChildren())
				{
					if (resource.GetMetaData() && resource.GetMetaData()->GetAssetType() == gallus::resources::AssetType::Folder)
					{
						return true;
					}
				}
				return false;
			}

			bool ExplorerFileUIView::SearchForPath(const fs::path& a_Path, ExplorerFileUIView*& a_pExplorerFile)
			{
				if (m_FileResource.GetPath() == a_Path)
				{
					a_pExplorerFile = this;
					return true;
				}
				for (ExplorerFileUIView& view : m_aChildren)
				{
					if (view.SearchForPath(a_Path, a_pExplorerFile))
					{
						return true;
					}
				}
				return false;
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE