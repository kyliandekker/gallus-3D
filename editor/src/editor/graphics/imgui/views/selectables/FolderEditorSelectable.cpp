#include "./FolderEditorSelectable.h"

// external
#include <imgui_helpers.h>
#include "imgui_system/font_icon.h"

// imgui
#include "imgui_system/ImGuiSystem.h"

// editor/file
#include "editor/file/FileResource.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// FolderEditorSelectable
	//---------------------------------------------------------------------
	FolderEditorSelectable::FolderEditorSelectable(ImGuiSystem& a_System, file::FileResource& a_FileResource) : BaseFileEditorSelectable(a_System, a_FileResource)
	{}

	//---------------------------------------------------------------------
	bool FolderEditorSelectable::HasFolders() const
	{
		for (const file::FileResource& resource : m_FileResource.GetChildren())
		{
			if (resource.GetAssetType() == resources::AssetType::Folder)
			{
				return true;
			}
		}
		return false;
	}

	//---------------------------------------------------------------------
	bool FolderEditorSelectable::IsFoldedOut() const
	{
		return m_bIsFoldedOut;
	}

	//---------------------------------------------------------------------
	void FolderEditorSelectable::SetFoldedOut(bool a_bIsFoldedOut)
	{
		m_bIsFoldedOut = a_bIsFoldedOut;
	}

	//---------------------------------------------------------------------
	void FolderEditorSelectable::RenderTree(bool& a_bClicked, bool& a_bRightClicked, bool a_bSelected, int a_iIndent, const ImVec2& a_vInitialPos, bool a_bInContextMenu)
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

		std::string	folderIcon = m_bIsFoldedOut ? font::ICON_FOLDER_OPEN : font::ICON_FOLDER;
		ImVec2 iconSize = ImGui::CalcTextSize(folderIcon.c_str());

		float verticalOffset = (buttonSize.y - iconSize.y) / 2.0f;

		ImVec2 contentStartPos = ImVec2(a_vInitialPos.x + (ImGui::GetStyle().IndentSpacing * a_iIndent) + (m_System.GetFontSize() * 2.0f), a_vInitialPos.y + verticalOffset);
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

		ImVec2 textSize = ImGui::CalcTextSize(m_sName.c_str());

		verticalOffset = (buttonSize.y - textSize.y) / 2.0f;

		ImGui::SetCursorPos(contentStartPos + ImVec2(m_System.GetFontSize() * 1.5f, 0));

		ImGui::Text(m_sName.c_str());

		ImGui::SetCursorPos(ImVec2(a_vInitialPos.x, a_vInitialPos.y + buttonSize.y));
	}
}