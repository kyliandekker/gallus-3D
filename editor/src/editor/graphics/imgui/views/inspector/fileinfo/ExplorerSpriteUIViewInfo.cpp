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
#include <editor/graphics/imgui/EditorWindowsConfig.h>

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
				m_TextureTypeDropdown.SetValue(graphics::dx12::TextureType::Texture2D);

				auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				m_pPreviewTexture = core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(m_ExplorerFileUIView.GetFileResource().GetPath().filename().generic_string(), cCommandQueue);
				m_pPreviewTexture->SetResourceCategory(gallus::resources::EngineResourceCategory::Editor);
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
				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SPRITE_EXPLORER_ITEM_TABLE_INSPECTOR"), m_Window.GetFramePadding());

				ImGuiWindow& window = m_Window;
				ExplorerFileUIView& explorerFileUIView = m_ExplorerFileUIView;
				StringDropdown<gallus::graphics::dx12::TextureType>& textureTypeDropdown = m_TextureTypeDropdown;
				resources::TextureMetaData* metaData = m_ExplorerFileUIView.GetFileResource().GetMetaData<resources::TextureMetaData>();
				std::shared_ptr<graphics::dx12::Texture> previewTex = m_pPreviewTexture;
				if (m_pPreviewTexture)
				{
					ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Width: ");
					},
						[previewTex]
					{
						ImGui::Text(std::to_string(previewTex->GetResourceDesc().Width).c_str());
					});
					ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Height: ");
					},
						[previewTex]
					{
						ImGui::Text(std::to_string(previewTex->GetResourceDesc().Height).c_str());
					});
					ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Channels: ");
					},
						[previewTex]
					{
						ImGui::Text(std::to_string(GetFormatChannelCount(previewTex->GetResourceDesc().Format)).c_str());
					});
				}
				ImGui::KeyValue([&window]
				{
					ImGui::AlignTextToFramePadding();
					ImGui::DisplayHeader(window.GetBoldFont(), "Type: ");
				},
					[&textureTypeDropdown, &explorerFileUIView, metaData]
				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (textureTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "TEXTURE_TYPE_SPRITE_EXPLORER_ITEM_INSPECTOR").c_str()))
					{
						metaData->SetTextureType(textureTypeDropdown.GetValue());
						metaData->Save(explorerFileUIView.GetFileResource().GetPath());
					}
				});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());

				float width = ImGui::GetContentRegionAvail().x;
				if (metaData->GetTextureType() == graphics::dx12::TextureType::SpriteSheet && ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_IMAGE + std::string(" Open Sprite Editor"), BUTTON_ID, "OPEN_SPRITE_EDITOR_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					SpriteEditorModal& spriteEditorModal = m_Window.GetWindowsConfig<EditorWindowsConfig>().GetSpriteEditorModal();

					spriteEditorModal.SetData(
						m_ExplorerFileUIView.GetFileResource()
					);
					spriteEditorModal.Show();
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
					auto spriteUvs = m_pPreviewTexture->GetSpriteUV(m_iCurrentSpriteIndex);

					spriteW = m_pPreviewTexture->GetSpriteRect(m_iCurrentSpriteIndex).width;
					spriteH = m_pPreviewTexture->GetSpriteRect(m_iCurrentSpriteIndex).height;
					uv0 = ImVec2(spriteUvs.uv0.x, spriteUvs.uv0.y);
					uv1 = ImVec2(spriteUvs.uv1.x, spriteUvs.uv1.y);
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
						if (m_iCurrentSpriteIndex < 0) m_iCurrentSpriteIndex = m_pPreviewTexture->GetSpriteRectsSize() - 1; // wrap around
					}
					ImGui::SameLine();
					if (ImGui::Button("Next"))
					{
						m_iCurrentSpriteIndex++;
						if (m_iCurrentSpriteIndex >= m_pPreviewTexture->GetSpriteRectsSize()) m_iCurrentSpriteIndex = 0; // wrap around
					}

					ImVec2 textPos = image_pos;
					textPos.x += drawW;
					std::string indexText = std::to_string(m_iCurrentSpriteIndex) + "/" + std::to_string(m_pPreviewTexture->GetSpriteRectsSize() - 1);
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
