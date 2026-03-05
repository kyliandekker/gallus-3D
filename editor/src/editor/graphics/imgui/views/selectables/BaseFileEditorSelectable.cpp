#include "BaseFileEditorSelectable.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "imgui_system/ImGuiSystem.h"
#include "imgui_system/font_icon.h"

// editor/file
#include "editor/file/FileResource.h"
#include "file/file_abstractions.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	BaseFileEditorSelectable::BaseFileEditorSelectable(ImGuiSystem& a_System, file::FileResource& a_FileResource) : ListItemEditorSelectable(a_System),
		m_FileResource(a_FileResource)
	{
		m_bShowRename = true;
		m_bShowDelete = true;
		m_bShowShowInExplorer = true;

		SetFileResourceProperties();
	}

	//---------------------------------------------------------------------
	void BaseFileEditorSelectable::SetFileResourceProperties()
	{
		SetName(m_FileResource.GetPath().filename().generic_string());
		m_AssetType = m_FileResource.GetAssetType();
		m_sIcon = AssetTypeToFileIcon(m_FileResource.GetAssetType());
	}

	//---------------------------------------------------------------------
	void BaseFileEditorSelectable::OnRename(const std::string& a_sName)
	{
		m_FileResource.Rename(a_sName);
	}

	//---------------------------------------------------------------------
	void BaseFileEditorSelectable::OnDelete()
	{
		m_FileResource.Delete();
	}

	//---------------------------------------------------------------------
	void BaseFileEditorSelectable::OnShowInExplorer()
	{
		file::OpenInExplorer(m_FileResource.GetPath().parent_path().lexically_normal());
	}
}