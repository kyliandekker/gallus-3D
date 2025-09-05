#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerShaderUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// graphics includes
#include "graphics/imgui/ImGuiWindow.h"

// editor includes
#include "editor/FileResource.h"
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
                    m_ExplorerFileUIView.GetFileResource().GetMetaData().GetAssetType(),
                    {
                        editor::AssetType::PixelShader,
                        editor::AssetType::VertexShader,
                    },
                    editor::AssetTypeToString
                );
            }

            void ExplorerShaderUIViewInfo::RenderSpecific()
            {
                ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type: ");
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x, 0));
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (m_AssetTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_SHADER_FILE_INSPECTOR").c_str()))
                {
                    m_ExplorerFileUIView.GetFileResource().GetMetaData().SetAssetType(m_AssetTypeDropdown.GetValue());
                }
                ImGui::PopStyleVar();
            }
        }
    }
}

#endif // _EDITOR
#endif // IMGUI_DISABLE