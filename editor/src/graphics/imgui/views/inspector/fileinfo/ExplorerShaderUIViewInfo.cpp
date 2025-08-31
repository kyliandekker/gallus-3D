#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerShaderUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include "graphics/imgui/ImGuiWindow.h"
#include "core/Tool.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/FileResource.h"

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
                    m_ExplorerFileUIView.GetFileResource().GetAssetType(),
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
                if (m_AssetTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_INSPECTOR").c_str()))
                {
                    m_ExplorerFileUIView.GetFileResource().SetAssetType(m_AssetTypeDropdown.GetValue());
                    rapidjson::Document doc;
                    doc.SetObject();
                    m_ExplorerFileUIView.GetFileResource().GetMetaData(doc);
                    m_ExplorerFileUIView.GetFileResource().SaveMetadata(doc, doc.GetAllocator());
                }
                ImGui::PopStyleVar();
            }
        }
    }
}

#endif // _EDITOR
#endif // IMGUI_DISABLE