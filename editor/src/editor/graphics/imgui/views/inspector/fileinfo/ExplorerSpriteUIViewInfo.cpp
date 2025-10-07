#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerSpriteUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// core includes
#include "editor/core/EditorEngine.h"

// graphics includes
#include "graphics/imgui/ImGuiWindow.h"

// graphics includes
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/imgui/font_icon.h"

// resources
#include "resources/FileResource.h"

// editor includes
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

                resources::TextureMetaData* metaData = m_ExplorerFileUIView.GetFileResource().GetMetaData<resources::TextureMetaData>();
                m_TextureTypeDropdown.Initialize(
                    metaData->GetTextureType(),
                    {
                        graphics::dx12::TextureType::Texture2D,
                        graphics::dx12::TextureType::SpriteSheet,
                    },
                    graphics::dx12::TextureTypeToString
                    );

				auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				m_pPreviewTexture = core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(m_ExplorerFileUIView.GetFileResource().GetPath().filename().generic_string(), cCommandQueue);
				m_pPreviewTexture->SetResourceCategory(gallus::core::EngineResourceCategory::Editor);
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

                    // 3 channels (rare, often unused � many APIs avoid 3-component formats)
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

                resources::TextureMetaData* metaData = m_ExplorerFileUIView.GetFileResource().GetMetaData<resources::TextureMetaData>();

                ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type: ");
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x, 0));
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (m_TextureTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_SHADER_FILE_INSPECTOR").c_str()))
                {
                    metaData->SetTextureType(m_TextureTypeDropdown.GetValue());
                    metaData->Save(m_ExplorerFileUIView.GetFileResource().GetPath());
                }
                ImGui::PopStyleVar();

                ImGui::NewLine();

                float width = ImGui::GetContentRegionAvail().x;
                if (metaData->GetTextureType() == graphics::dx12::TextureType::SpriteSheet && ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_IMAGE + std::string(" Open Sprite Editor"), BUTTON_ID, "OPEN_SPRITE_EDITOR_INSPECTOR").c_str(), ImVec2(width, 0)))
                {
                    SpriteEditorModal* modal = dynamic_cast<SpriteEditorModal*>(m_Window.GetModal((int) EDITOR_MODAL::EDITOR_MODAL_SPRITE_EDITOR));
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
                {
                    return;
                }

                // Sprite dimensions
                float spriteW = 0.0f;
                float spriteH = 0.0f;
                ImVec2 uv0, uv1;

                // Get TextureMetaData
                resources::TextureMetaData* metaData = m_ExplorerFileUIView.GetFileResource().GetMetaData<resources::TextureMetaData>();
                const auto& sprites = metaData->GetSprites();

                ImVec2 texturePos = ImGui::GetCursorPos();
                if (m_pPreviewTexture->GetTextureType() == graphics::dx12::TextureType::Texture2D)
                {
                    // Full texture
                    spriteW = static_cast<float>(m_pPreviewTexture->GetResourceDesc().Width);
                    spriteH = static_cast<float>(m_pPreviewTexture->GetResourceDesc().Height);
                    uv0 = { 0.0f, 0.0f };
                    uv1 = { 1.0f, 1.0f };
                }
                else
                {
                    const auto& sprite = sprites[m_iCurrentSpriteIndex];
                    spriteW = static_cast<float>(sprite.width);
                    spriteH = static_cast<float>(sprite.height);

                    const float texWidth = static_cast<float>(m_pPreviewTexture->GetResourceDesc().Width);
                    const float texHeight = static_cast<float>(m_pPreviewTexture->GetResourceDesc().Height);

                    uv0 = { sprite.x / texWidth, sprite.y / texHeight };                              // top-left
                    uv1 = { (sprite.x + sprite.width) / texWidth, (sprite.y + sprite.height) / texHeight }; // bottom-right
                }

                ImVec2 avail = ImGui::GetContentRegionAvail();
                ImVec2 padding = ImVec2();
                avail.x -= padding.x * 2.0f;
                avail.y -= padding.y * 2.0f;

                // Fit inside available space (keep aspect ratio)
                float scale = std::min(avail.x / spriteW, avail.y / spriteH);
                float drawW = spriteW * scale;
                float drawH = spriteH * scale;

                // Center horizontally
                float cursorX = ImGui::GetCursorPosX() + (avail.x - drawW) * 0.5f;
                ImGui::SetCursorPosX(cursorX);

                // Center horizontally
                ImGui::SetCursorPosX(cursorX);

                // Draw image
                ImVec2 image_pos = ImGui::GetCursorScreenPos();
                ImGui::Image((ImTextureID) m_pPreviewTexture->GetGPUHandle().ptr, ImVec2(drawW, drawH), uv0, uv1);

                // Draw border
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->AddRect(image_pos, ImVec2(image_pos.x + drawW, image_pos.y + drawH),
                    ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border]));
                
                if (m_pPreviewTexture->GetTextureType() == graphics::dx12::TextureType::SpriteSheet)
                {
                    ImVec2 buttonPos = image_pos;
                    buttonPos.y += m_Window.GetFramePadding().y;
                    buttonPos.x += m_Window.GetFramePadding().x;
                    ImGui::SetCursorScreenPos(buttonPos);
                    // Navigation buttons
                    if (ImGui::Button("Prev"))
                    {
                        m_iCurrentSpriteIndex--;
                        if (m_iCurrentSpriteIndex < 0) m_iCurrentSpriteIndex = sprites.size() - 1; // wrap around
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Next"))
                    {
                        m_iCurrentSpriteIndex++;
                        if (m_iCurrentSpriteIndex >= sprites.size()) m_iCurrentSpriteIndex = 0; // wrap around
                    }

                    ImVec2 textPos = image_pos;
                    textPos.x += drawW;
                    std::string indexText = std::to_string(m_iCurrentSpriteIndex) + "/" + std::to_string(sprites.size() - 1);
                    textPos.x -= ImGui::CalcTextSize(indexText.c_str()).x;
                    textPos.x -= m_Window.GetFramePadding().x;
                    textPos.y += m_Window.GetFramePadding().y;
                    ImGui::SetCursorScreenPos(textPos);
                    ImGui::Text(indexText.c_str());
                }
            }
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE