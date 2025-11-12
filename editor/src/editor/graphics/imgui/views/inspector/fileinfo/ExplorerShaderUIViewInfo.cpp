#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerShaderUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// graphics includes
#include "graphics/imgui/ImGuiWindow.h"

// resources
#include "resources/FileResource.h"

// editor includes
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			ExplorerShaderUIViewInfo::ExplorerShaderUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ExplorerFileUIViewInfo(a_Window, a_ExplorerFileUIView),
				m_AssetTypeDropdown(a_Window)
			{
				m_AssetTypeDropdown.Initialize(
					m_ExplorerFileUIView.GetFileResource().GetMetaData()->GetAssetType(),
					{
						resources::AssetType::PixelShader,
						resources::AssetType::VertexShader,
					},
					resources::AssetTypeToString
				);
			}

			void ExplorerShaderUIViewInfo::RenderSpecific()
			{
				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SHADER_EXPLORER_ITEM_TABLE_INSPECTOR"), m_Window.GetFramePadding());

				ImGuiWindow& window = m_Window;
				ExplorerFileUIView& explorerFileUIView = m_ExplorerFileUIView;
				StringDropdown<gallus::resources::AssetType>& assetTypeDropdown = m_AssetTypeDropdown;
				ImGui::KeyValue([&window]
				{
					ImGui::AlignTextToFramePadding();
					ImGui::DisplayHeader(window.GetBoldFont(), "Type: ");
				},
					[&assetTypeDropdown, &explorerFileUIView]
				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (assetTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_SHADER_EXPLORER_ITEM_INSPECTOR").c_str()))
					{
						explorerFileUIView.GetFileResource().GetMetaData()->SetAssetType(assetTypeDropdown.GetValue());
					}
				});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE
