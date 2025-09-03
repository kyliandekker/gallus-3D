#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerSpriteUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// core includes
#include "core/Tool.h"

// graphics includes
#include "graphics/imgui/ImGuiWindow.h"

// graphics includes
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"

// editor includes
#include "editor/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			ExplorerSpriteUIViewInfo::ExplorerSpriteUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ExplorerFileUIViewInfo(a_Window, a_ExplorerFileUIView),
                m_TextureTypeDropdown(a_Window)
			{
				m_bShowPreview = true;

                m_TextureTypeDropdown.Initialize(
                    graphics::dx12::TextureType::Texture2D,
                    {
                        graphics::dx12::TextureType::Texture2D,
                        graphics::dx12::TextureType::SpriteSheet,
                    },
                    graphics::dx12::TextureTypeToString
                    );

				auto cCommandQueue = core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				auto cCommandList = cCommandQueue->GetCommandList();
				m_pPreviewTexture = core::TOOL->GetResourceAtlas().LoadTexture(m_ExplorerFileUIView.GetFileResource().GetPath().filename().generic_string(), cCommandList);
				m_pPreviewTexture->SetResourceCategory(gallus::core::EngineResourceCategory::Editor);
				cCommandQueue->ExecuteCommandList(cCommandList);
				cCommandQueue->Flush();
			}

			ExplorerSpriteUIViewInfo::~ExplorerSpriteUIViewInfo()
			{
				
			}
            
            int GetFormatChannelCount(DXGI_FORMAT format)
            {
                switch (format)
                {
                    // 1 channel
                    case DXGI_FORMAT_R32_FLOAT:
                    case DXGI_FORMAT_R32_UINT:
                    case DXGI_FORMAT_R32_SINT:
                    case DXGI_FORMAT_R16_FLOAT:
                    case DXGI_FORMAT_R16_UNORM:
                    case DXGI_FORMAT_R16_UINT:
                    case DXGI_FORMAT_R16_SNORM:
                    case DXGI_FORMAT_R16_SINT:
                    case DXGI_FORMAT_R8_UNORM:
                    case DXGI_FORMAT_R8_UINT:
                    case DXGI_FORMAT_R8_SNORM:
                    case DXGI_FORMAT_R8_SINT:
                        return 1;

                    // 2 channels
                    case DXGI_FORMAT_R32G32_FLOAT:
                    case DXGI_FORMAT_R32G32_UINT:
                    case DXGI_FORMAT_R32G32_SINT:
                    case DXGI_FORMAT_R16G16_FLOAT:
                    case DXGI_FORMAT_R16G16_UNORM:
                    case DXGI_FORMAT_R16G16_UINT:
                    case DXGI_FORMAT_R16G16_SNORM:
                    case DXGI_FORMAT_R16G16_SINT:
                    case DXGI_FORMAT_R8G8_UNORM:
                    case DXGI_FORMAT_R8G8_UINT:
                    case DXGI_FORMAT_R8G8_SNORM:
                    case DXGI_FORMAT_R8G8_SINT:
                        return 2;

                    // 3 channels (rare, often unused — many APIs avoid 3-component formats)
                    case DXGI_FORMAT_R32G32B32_FLOAT:
                    case DXGI_FORMAT_R32G32B32_UINT:
                    case DXGI_FORMAT_R32G32B32_SINT:
                        return 3;

                    // 4 channels
                    case DXGI_FORMAT_R32G32B32A32_FLOAT:
                    case DXGI_FORMAT_R32G32B32A32_UINT:
                    case DXGI_FORMAT_R32G32B32A32_SINT:
                    case DXGI_FORMAT_R16G16B16A16_FLOAT:
                    case DXGI_FORMAT_R16G16B16A16_UNORM:
                    case DXGI_FORMAT_R16G16B16A16_UINT:
                    case DXGI_FORMAT_R16G16B16A16_SNORM:
                    case DXGI_FORMAT_R16G16B16A16_SINT:
                    case DXGI_FORMAT_R8G8B8A8_UNORM:
                    case DXGI_FORMAT_R8G8B8A8_UINT:
                    case DXGI_FORMAT_R8G8B8A8_SNORM:
                    case DXGI_FORMAT_R8G8B8A8_SINT:
                    case DXGI_FORMAT_B8G8R8A8_UNORM:
                    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                        return 4;

                    default:
                        return 0; // Unknown/unsupported format
                }
            }

			void ExplorerSpriteUIViewInfo::RenderSpecific()
			{
				if (m_pPreviewTexture)
				{
					ImGui::DisplayHeader(m_Window.GetBoldFont(), "Width: ");
					ImGui::SameLine();
					ImGui::Text(std::to_string(m_pPreviewTexture->GetResourceDesc().Width).c_str());

					ImGui::DisplayHeader(m_Window.GetBoldFont(), "Height: ");
					ImGui::SameLine();
					ImGui::Text(std::to_string(m_pPreviewTexture->GetResourceDesc().Height).c_str());

					ImGui::DisplayHeader(m_Window.GetBoldFont(), "Channels: ");
					ImGui::SameLine();
					ImGui::Text(std::to_string(GetFormatChannelCount(m_pPreviewTexture->GetResourceDesc().Format)).c_str());
				}

                ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type: ");
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x, 0));
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (m_TextureTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_SHADER_FILE_INSPECTOR").c_str()))
                {
                    //m_ExplorerFileUIView.GetFileResource().SetAssetType(m_TextureTypeDropdown.GetValue());
                }
                ImGui::PopStyleVar();
			}

			void ExplorerSpriteUIViewInfo::RenderPreview()
			{
				if (m_pPreviewTexture && m_pPreviewTexture->CanBeDrawn())
				{
					const float height_new = ImGui::GetContentRegionAvail().y;
					const float width = (m_pPreviewTexture->GetResourceDesc().Width * (1.0f / m_pPreviewTexture->GetResourceDesc().Height * height_new));

					float offset = (ImGui::GetContentRegionAvail().x - width) / 2;
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
					ImVec2 image_pos = ImGui::GetCursorScreenPos();
					ImGui::Image((ImTextureID) m_pPreviewTexture->GetGPUHandle().ptr, ImVec2(width, height_new));

					// Draw border
					ImDrawList* draw_list = ImGui::GetForegroundDrawList();
					draw_list->AddRect(image_pos, ImVec2(image_pos.x + width, image_pos.y + height_new), ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border])); // White border
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE