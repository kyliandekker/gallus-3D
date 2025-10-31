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
#include "editor/core/EditorEngine.h"
#include "resources/AssetType.h"
#include "editor/graphics/imgui/modals/FilePickerModal.h"

// gameplay includes
#include "gameplay/Game.h"
#include "gameplay/systems/TransformSystem.h"

// resources
#include "resources/FileResource.h"
#include <editor/graphics/imgui/EditorWindowsConfig.h>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			char m_sPixelShaderName[128];
			char m_sVertexShaderName[128];
			char m_TextureName[128];
			SpriteComponentUIView::SpriteComponentUIView(ImGuiWindow& a_Window, gameplay::SpriteComponent& a_SpriteComponent, gameplay::SpriteSystem& a_System) : ComponentUIView(a_Window, a_SpriteComponent, a_System), 
				m_ColorView(a_Window)
			{
				m_bShowPreview = true;
				m_iPreviewPriority = 4;
			}

			void SpriteComponentUIView::RenderInner()
			{
				memset(m_sPixelShaderName, 0, sizeof(m_sPixelShaderName));
				strncpy(m_sPixelShaderName, m_Component.GetShader()->GetPixelShader()->GetName().c_str(), sizeof(m_sPixelShaderName));
				m_sPixelShaderName[sizeof(m_sPixelShaderName) - 1] = '\0';

				memset(m_sVertexShaderName, 0, sizeof(m_sVertexShaderName));
				strncpy(m_sVertexShaderName, m_Component.GetShader()->GetVertexShader()->GetName().c_str(), sizeof(m_sVertexShaderName));
				m_sVertexShaderName[sizeof(m_sVertexShaderName) - 1] = '\0';

				memset(m_TextureName, 0, sizeof(m_TextureName));
				strncpy(m_TextureName, m_Component.GetTexture()->GetName().c_str(), sizeof(m_TextureName));
				m_TextureName[sizeof(m_TextureName) - 1] = '\0';

				FilePickerModal& filePickerModal = m_Window.GetWindowsConfig<EditorWindowsConfig>().GetFilePickerModal();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SPRITE_COMPONENT_TABLE_INSEPCTOR"), m_Window.GetFramePadding());
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Pixel Shader: ");
					},
					[this, &buttonSize, &filePickerModal]
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
						ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_PIXEL_SHADER_NAME_INPUT_INSPECTOR").c_str(), m_sPixelShaderName, sizeof(m_sPixelShaderName), ImGuiInputTextFlags_ReadOnly);
						ImGui::PopItemFlag();
						ImGui::SameLine();
						if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "SPRITE_COMPONENT_PIXEL_SHADER_INSPECTOR").c_str(), buttonSize))
						{
							filePickerModal.SetData(
								[this](int success, gallus::resources::FileResource& resource)
								{
									if (success == 1)
									{
										m_Component.SetShader(
											core::EDITOR_ENGINE->GetResourceAtlas().LoadShaderBind(
												m_Component.GetTexture()->GetName(),
												core::EDITOR_ENGINE->GetResourceAtlas().LoadPixelShader(resource.GetPath().filename().generic_string()).get(),
												m_Component.GetShader()->GetVertexShader()
											).get());

										core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
									}
								},
								std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::PixelShader }
							);
							filePickerModal.Show();
						}
					});
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Vertex Shader: ");
					},
					[this, &buttonSize, &filePickerModal]
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
						ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_VERTEX_SHADER_NAME_INPUT_INSPECTOR").c_str(), m_sVertexShaderName, sizeof(m_sVertexShaderName), ImGuiInputTextFlags_ReadOnly);
						ImGui::PopItemFlag();
						ImGui::SameLine();
						if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "SPRITE_COMPONENT_VERTEX_SHADER_INSPECTOR").c_str(), buttonSize))
						{
							filePickerModal.SetData(
								[this](int success, gallus::resources::FileResource& resource)
								{
									if (success == 1)
									{
										m_Component.SetShader(
											core::EDITOR_ENGINE->GetResourceAtlas().LoadShaderBind(
												m_Component.GetTexture()->GetName(),
												m_Component.GetShader()->GetPixelShader(),
												core::EDITOR_ENGINE->GetResourceAtlas().LoadVertexShader(resource.GetPath().filename().generic_string()).get()
											).get());

										core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
									}
								},
								std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::VertexShader }
							);
							filePickerModal.Show();
						}
					});
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Texture: ");
					},
					[this, &buttonSize, &filePickerModal]
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
						ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_TEXTURE_NAME_INPUT_INSPECTOR").c_str(), m_TextureName, sizeof(m_TextureName), ImGuiInputTextFlags_ReadOnly);
						ImGui::PopItemFlag();
						ImGui::SameLine();
						if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "SPRITE_COMPONENT_TEXTURE_INSPECTOR").c_str(), buttonSize))
						{
							filePickerModal.SetData(
								[this](int success, gallus::resources::FileResource& resource)
								{
									if (success == 1)
									{
										auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
										m_Component.SetTexture(core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(resource.GetPath().filename().generic_string(), cCommandQueue).get());

										core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
									}
								},
								std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::Sprite }
							);
							filePickerModal.Show();
						}
					});
				ImGui::PopStyleVar();
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Sprite Index: ");
					},
					[this]
					{
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						int spriteIndex = m_Component.GetSpriteIndex();
						if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_CURRENT_SPRITE_INDEX_INSPECTOR").c_str(), &spriteIndex, 1, 0, m_Component.GetTexture()->GetSpriteRectsSize() - 1))
						{
							m_Component.SetSpriteIndex(spriteIndex);
						}
					});
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Color: ");
					},
					[this]
					{
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						m_ColorView.SetValue(m_Component.GetColor());
						if (m_ColorView.Render("SPRITE_COMPONENT_COLOR_INSPECTOR"))
						{
							m_Component.SetColor(m_ColorView.GetValue());
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());

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