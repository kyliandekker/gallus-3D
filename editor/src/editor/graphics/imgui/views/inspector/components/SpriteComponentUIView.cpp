#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "SpriteComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics includes
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12ShaderBind.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/imgui/font_icon.h"

// editor includes
#include "editor/core/EditorTool.h"
#include "resources/AssetType.h"
#include "editor/FileResource.h"
#include "editor/graphics/imgui/modals/FilePickerModal.h"

// gameplay includes
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			char m_PixelShaderName[128];
			char m_VertexShaderName[128];
			char m_TextureName[128];
			SpriteComponentUIView::SpriteComponentUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, gameplay::SpriteComponent& a_SpriteComponent, gameplay::SpriteSystem& a_System) : ComponentUIView(a_Window, a_EntityID, a_SpriteComponent, a_System), m_SizeView(a_Window)
			{
				m_bShowPreview = true;
				m_iPreviewPriority = 4;
			}

			void SpriteComponentUIView::RenderInner()
			{
				memset(m_PixelShaderName, 0, sizeof(m_PixelShaderName));
				strncpy(m_PixelShaderName, m_Component.GetShader()->GetPixelShader()->GetName().c_str(), sizeof(m_PixelShaderName));
				m_PixelShaderName[sizeof(m_PixelShaderName) - 1] = '\0';

				memset(m_VertexShaderName, 0, sizeof(m_PixelShaderName));
				strncpy(m_VertexShaderName, m_Component.GetShader()->GetVertexShader()->GetName().c_str(), sizeof(m_VertexShaderName));
				m_VertexShaderName[sizeof(m_VertexShaderName) - 1] = '\0';

				memset(m_TextureName, 0, sizeof(m_PixelShaderName));
				strncpy(m_TextureName, m_Component.GetTexture()->GetName().c_str(), sizeof(m_TextureName));
				m_TextureName[sizeof(m_TextureName) - 1] = '\0';

				FilePickerModal* modal = dynamic_cast<FilePickerModal*>(m_Window.GetModal(0));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Pixel Shader: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_PIXEL_SHADER_NAME_INPUT").c_str(), m_PixelShaderName, sizeof(m_PixelShaderName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "MESH_COMPONENT_PIXEL_SHADER").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, gallus::editor::FileResource& resource)
							{
								if (success == 1)
								{
									m_Component.SetShader(
										core::TOOL->GetResourceAtlas().LoadShaderBind(
										core::TOOL->GetResourceAtlas().LoadPixelShader(resource.GetPath().filename().generic_string()).get(),
										m_Component.GetShader()->GetVertexShader()
									).get());
								}
							},
							std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::PixelShader }
						);
						modal->Show();
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Vertex Shader: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_VERTEX_SHADER_NAME_INPUT").c_str(), m_VertexShaderName, sizeof(m_VertexShaderName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "MESH_COMPONENT_VERTEX_SHADER").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, gallus::editor::FileResource& resource)
							{
								if (success == 1)
								{
									m_Component.SetShader(
										core::TOOL->GetResourceAtlas().LoadShaderBind(
											m_Component.GetShader()->GetPixelShader(), 
											core::TOOL->GetResourceAtlas().LoadVertexShader(resource.GetPath().filename().generic_string()).get()
									).get());
									
									gameplay::GAME->GetScene().SetIsDirty(true);
								}
							},
							std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::VertexShader }
						);
						modal->Show();
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Texture: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_TEXTURE_NAME_INPUT").c_str(), m_TextureName, sizeof(m_TextureName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_IMAGE, BUTTON_ID, "MESH_COMPONENT_TEXTURE").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, gallus::editor::FileResource& resource)
							{
								if (success == 1)
								{
									auto cCommandQueue = core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
									auto cCommandList = cCommandQueue->GetCommandList();
									m_Component.SetTexture(core::TOOL->GetResourceAtlas().LoadTexture(resource.GetPath().filename().generic_string(), cCommandList).get());
									cCommandQueue->ExecuteCommandList(cCommandList);
									cCommandQueue->Flush();

									gameplay::GAME->GetScene().SetIsDirty(true);
								}
							},
							std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::Texture }
						);
						modal->Show();
					}
				}

				if (m_Component.GetTexture() && m_Component.GetTexture()->GetTextureType() == graphics::dx12::TextureType::SpriteSheet)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

					int spriteIndex = m_Component.GetSpriteIndex();
					if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "MESH_COMPONENT_CURRENT_SPRITE_INDEX").c_str(), &spriteIndex, 1, 0, m_Component.GetTexture()->GetSpriteRectsSize() - 1))
					{
						m_Component.SetSpriteIndex(spriteIndex);
					}
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			std::string SpriteComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void SpriteComponentUIView::RenderPreview()
			{
				if (!m_Component.GetTexture() || !m_Component.GetTexture()->CanBeDrawn())
				{
					return;
				}

				// Sprite dimensions
				float spriteW = 0.0f;
				float spriteH = 0.0f;
				ImVec2 uv0, uv1;

				// Get TextureMetaData
				ImVec2 texturePos = ImGui::GetCursorPos();
				if (m_Component.GetTexture()->GetTextureType() == graphics::dx12::TextureType::Texture2D)
				{
					// Full texture
					spriteW = static_cast<float>(m_Component.GetTexture()->GetResourceDesc().Width);
					spriteH = static_cast<float>(m_Component.GetTexture()->GetResourceDesc().Height);
					uv0 = { 0.0f, 0.0f };
					uv1 = { 1.0f, 1.0f };
				}
				else
				{
					const auto& sprite = m_Component.GetTexture()->GetSpriteRect(m_Component.GetSpriteIndex());
					spriteW = static_cast<float>(sprite.width);
					spriteH = static_cast<float>(sprite.height);

					const float texWidth = static_cast<float>(m_Component.GetTexture()->GetResourceDesc().Width);
					const float texHeight = static_cast<float>(m_Component.GetTexture()->GetResourceDesc().Height);

					uv0 = { sprite.x / texWidth, sprite.y / texHeight };                              // top-left
					uv1 = { (sprite.x + sprite.width) / texWidth, (sprite.y + sprite.height) / texHeight }; // bottom-right
				}

				ImGui::SetCursorPos(texturePos);
				// Available region (minus padding)
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
				ImGui::Image((ImTextureID) m_Component.GetTexture()->GetGPUHandle().ptr, ImVec2(drawW, drawH), uv0, uv1);

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