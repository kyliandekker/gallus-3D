#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerAudioUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/FileResource.h"

namespace gallus
{
    namespace graphics
    {
        namespace imgui
        {
            ExplorerAudioUIViewInfo::ExplorerAudioUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ExplorerFileUIViewInfo(a_Window, a_ExplorerFileUIView),
                m_AssetTypeDropdown(a_Window)
            {
                m_AssetTypeDropdown.Initialize(
                    m_ExplorerFileUIView.GetFileResource().GetAssetType(),
                    {
                        editor::AssetType::Sound,
                        editor::AssetType::Song,
                        editor::AssetType::VO,
                    },
                    editor::AssetTypeToString
                    );
            }

            void ExplorerAudioUIViewInfo::RenderSpecific()
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