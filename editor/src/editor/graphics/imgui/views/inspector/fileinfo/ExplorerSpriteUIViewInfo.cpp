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
#include "graphics/imgui/font_icon.h"

// editor includes
#include "editor/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"
#include "resources/metadata/TextureMetaData.h"
#include "editor/graphics/imgui/modals/SpriteEditorModal.h"

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

                resources::TextureMetaData& metaData = m_ExplorerFileUIView.GetFileResource().GetMetaData<resources::TextureMetaData>();
                m_TextureTypeDropdown.Initialize(
                    metaData.GetTextureType(),
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

                resources::TextureMetaData& metaData = m_ExplorerFileUIView.GetFileResource().GetMetaData<resources::TextureMetaData>();

                ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type: ");
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x, 0));
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (m_TextureTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_SHADER_FILE_INSPECTOR").c_str()))
                {
                    metaData.SetTextureType(m_TextureTypeDropdown.GetValue());
                    metaData.Save(m_ExplorerFileUIView.GetFileResource().GetPath());
                }
                ImGui::PopStyleVar();

                ImGui::NewLine();

                float width = ImGui::GetContentRegionAvail().x;
                if (metaData.GetTextureType() == graphics::dx12::TextureType::SpriteSheet && ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_IMAGE + std::string(" Open Sprite Editor"), BUTTON_ID, "OPEN_SPRITE_EDITOR_INSPECTOR").c_str(), ImVec2(width, 0)))
                {
                    SpriteEditorModal* modal = dynamic_cast<SpriteEditorModal*>(m_Window.GetModal(1));
                    if (modal)
                    {
                        modal->SetData(
                            m_ExplorerFileUIView.GetFileResource()
                        );
                        modal->Show();
                    }
                }
			}

            void ExplorerSpriteUIViewInfo::RenderPreview()
            {
                if (!m_pPreviewTexture || !m_pPreviewTexture->CanBeDrawn())
                    return;

                // Get TextureMetaData
                resources::TextureMetaData& metaData = m_ExplorerFileUIView.GetFileResource().GetMetaData<resources::TextureMetaData>();
                const auto& sprites = metaData.GetSprites();

                static int currentSpriteIndex = 0;
                const int maxIndex = static_cast<int>(sprites.size());

                // Navigation buttons
                if (ImGui::Button("Prev"))
                {
                    currentSpriteIndex--;
                    if (currentSpriteIndex < 0) currentSpriteIndex = maxIndex; // wrap around
                }
                ImGui::SameLine();
                if (ImGui::Button("Next"))
                {
                    currentSpriteIndex++;
                    if (currentSpriteIndex > maxIndex) currentSpriteIndex = 0; // wrap around
                }

                ImGui::Text("Showing sprite %d/%d", currentSpriteIndex, maxIndex);
                ImGui::Separator();

                // Available region (minus padding)
                ImVec2 avail = ImGui::GetContentRegionAvail();
                ImVec2 padding = ImVec2();
                avail.x -= padding.x * 2.0f;
                avail.y -= padding.y * 2.0f;

                // Sprite dimensions
                float spriteW = 0.0f;
                float spriteH = 0.0f;
                ImVec2 uv0, uv1;

                if (currentSpriteIndex == 0)
                {
                    // Full texture
                    spriteW = static_cast<float>(m_pPreviewTexture->GetResourceDesc().Width);
                    spriteH = static_cast<float>(m_pPreviewTexture->GetResourceDesc().Height);
                    uv0 = { 0.0f, 0.0f };
                    uv1 = { 1.0f, 1.0f };
                }
                else
                {
                    // Sprite rect
                    const auto& sprite = sprites[currentSpriteIndex - 1];
                    spriteW = static_cast<float>(sprite.width);
                    spriteH = static_cast<float>(sprite.height);

                    const float texWidth = static_cast<float>(m_pPreviewTexture->GetResourceDesc().Width);
                    const float texHeight = static_cast<float>(m_pPreviewTexture->GetResourceDesc().Height);

                    uv0 = { sprite.x / texWidth, sprite.y / texHeight };                              // top-left
                    uv1 = { (sprite.x + sprite.width) / texWidth, (sprite.y + sprite.height) / texHeight }; // bottom-right
                }

                // Fit inside available space (keep aspect ratio)
                float scale = std::min(avail.x / spriteW, avail.y / spriteH);
                float drawW = spriteW * scale;
                float drawH = spriteH * scale;

                // Center horizontally
                float cursorX = ImGui::GetCursorPosX() + (avail.x - drawW) * 0.5f;
                ImGui::SetCursorPosX(cursorX);

                // Draw image
                ImVec2 image_pos = ImGui::GetCursorScreenPos();
                ImGui::Image((ImTextureID) m_pPreviewTexture->GetGPUHandle().ptr, ImVec2(drawW, drawH), uv0, uv1);

                // Draw border
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->AddRect(image_pos, ImVec2(image_pos.x + drawW, image_pos.y + drawH),
                    ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border]));
            }


		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE