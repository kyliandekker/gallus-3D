#include "FileEditorSelectable.h"

// external
#include <imgui/imgui_helpers.h>

// imgui
#include "imgui_system/ImGuiSystem.h"
#include "imgui_system/font_icon.h"

// resources
#include "resources/AssetType.h"

// editor/file
#include "editor/file/FileResource.h"

#include "editor/graphics/imgui/views/selectables/file/FileTypeEditorSelectable.h"

namespace gallus::graphics::imgui
{
	FileEditorSelectable::FileEditorSelectable(ImGuiSystem& a_System, file::FileResource& a_FileResource) : BaseFileEditorSelectable(a_System, a_FileResource)
	{
	}

	//---------------------------------------------------------------------
	void FileEditorSelectable::OnSelect()
	{
		switch (m_FileResource.GetAssetType())
		{
			case resources::AssetType::Folder:
			{
				m_pFileEditorSelectable = new FileTypeEditorSelectable(m_System, *this);
				break;
			}
			case resources::AssetType::Scene:
			{
				m_pFileEditorSelectable = new SceneFileTypeEditorSelectable(m_System, *this);
				break;
			}
			case resources::AssetType::Texture:
			{
				m_pFileEditorSelectable = new TextureFileTypeEditorSelectable(m_System, *this);
				break;
			}
			case resources::AssetType::Sound:
			{
				m_pFileEditorSelectable = new AudioFileTypeEditorSelectable(m_System, *this);
				break;
			}
			case resources::AssetType::Song:
			{
				m_pFileEditorSelectable = new AudioFileTypeEditorSelectable(m_System, *this);
				break;
			}
			case resources::AssetType::VO:
			{
				m_pFileEditorSelectable = new AudioFileTypeEditorSelectable(m_System, *this);
				break;
			}
			case resources::AssetType::Animation:
			{
				m_pFileEditorSelectable = new AnimationFileTypeEditorSelectable(m_System, *this);
				break;
			}
			case resources::AssetType::PixelShader:
			{
				m_pFileEditorSelectable = new ShaderFileTypeEditorSelectable(m_System, *this);
				break;
			}
			case resources::AssetType::VertexShader:
			{
				m_pFileEditorSelectable = new ShaderFileTypeEditorSelectable(m_System, *this);
				break;
			}
			case resources::AssetType::Prefab:
			{
				m_pFileEditorSelectable = new PrefabFileTypeEditorSelectable(m_System, *this);
				break;
			}
			case resources::AssetType::Mesh:
			{
				m_pFileEditorSelectable = nullptr;
				break;
			}
			case resources::AssetType::Material:
			{
				m_pFileEditorSelectable = new MaterialFileTypeEditorSelectable(m_System, *this);
				break;
			}
		}
	}

	//---------------------------------------------------------------------
	void FileEditorSelectable::OnDeselect()
	{
		delete m_pFileEditorSelectable;
	}

	//---------------------------------------------------------------------
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

	//---------------------------------------------------------------------
	void FileEditorSelectable::RenderGrid(const ImVec2& m_vSize, bool& a_bClicked, bool& a_bRightClicked, bool& a_bDoubleClicked, bool a_bSelected, bool a_bInContextMenu)
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
			std::string name = m_sName;
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
			ImRect buttonRect = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax() };
			EnableDragFunctionality();

			ImGui::PushFont(m_System.GetBigIconFont());

			ImVec2 assetTypeIconSize = ImGui::CalcTextSize(m_sIcon.c_str()); 

			// Calculate position to center the icon
			float horizontalOffset = (m_vSize.x - assetTypeIconSize.x) / 2.0f;
			ImVec2 assetTypeIconPos = ImVec2(buttonRect.Min.x + horizontalOffset, buttonRect.Min.y + m_System.GetFontSize());
			ImGui::SetCursorScreenPos(assetTypeIconPos);

			ImVec4 assetTypeIconColor = AssetTypeToColor(m_FileResource.GetAssetType());
			if (a_bSelected)
			{
				assetTypeIconColor = ImVec4(1, 1, 1, 1);
			}
			ImGui::TextColored(assetTypeIconColor, m_sIcon.c_str());
			ImGui::PopFont();

			// Draw file name under icon
			std::string displayedName = TruncateTextToFit(name, m_vSize.x - m_System.GetItemSpacing().x); // little padding

			ImVec2 nameTextSize = ImGui::CalcTextSize(displayedName.c_str());
			float nameTextX = initialScreenPos.x + (m_vSize.x - nameTextSize.x) / 2.0f;
			float nameTextY = assetTypeIconPos.y + assetTypeIconSize.y + m_System.GetItemSpacing().y; // gap below icon

			ImGui::SetCursorScreenPos(ImVec2(nameTextX, nameTextY));
			ImGui::TextUnformatted(displayedName.c_str());
		}
		ImGui::EndChild();
	}

	//---------------------------------------------------------------------
	void FileEditorSelectable::OnDoubleClicked()
	{
		if (!m_pFileEditorSelectable)
		{
			return;
		}

		return m_pFileEditorSelectable->OpenFile(*this);
	}

	//---------------------------------------------------------------------
	bool FileEditorSelectable::RenderEditorFields()
	{
		if (!m_pFileEditorSelectable)
		{
			return false;
		}

		m_pFileEditorSelectable->Render(*this);
		return true;
	}

    void FileEditorSelectable::EnableDragFunctionality()
    {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_System.GetFramePadding());
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			file::FileResource* pFile = &m_FileResource;
			ImGui::SetDragDropPayload("EXPLORER_ITEM", &pFile, sizeof(file::FileResource*));
			ImGui::TextColored(AssetTypeToColor(m_FileResource.GetAssetType()), m_sIcon.c_str());
			ImGui::SameLine();
			ImGui::Text(m_sName.c_str());
			ImGui::EndDragDropSource();
		}
		ImGui::PopStyleVar();
	}
}