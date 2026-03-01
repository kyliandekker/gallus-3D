#include "EngineResourceEditorSelectable.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "imgui_system/ImGuiSystem.h"
#include "imgui_system/font_icon.h"

// editor
#include "resources/EngineResource.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// EngineResourceEditorSelectable
	//---------------------------------------------------------------------
	EngineResourceEditorSelectable::EngineResourceEditorSelectable(ImGuiSystem& a_System, std::weak_ptr<resources::EngineResource> a_pResource) : ListItemEditorSelectable(a_System), m_pResource(a_pResource)
	{
		m_bShowRename = true;
		m_bShowDelete = true;
		m_bShowShowInExplorer = true;

		SetEngineResourceProperties();

		if (std::shared_ptr<resources::EngineResource> pResource = m_pResource.lock())
		{
			SetName(pResource->GetName());
		}
	}

	//---------------------------------------------------------------------
	void EngineResourceEditorSelectable::SetEngineResourceProperties()
	{
		if (std::shared_ptr<resources::EngineResource> pResource = m_pResource.lock())
		{
			m_AssetType = pResource->GetAssetType();
			m_sIcon = AssetTypeToFileIcon(m_AssetType);
		}
	}
}