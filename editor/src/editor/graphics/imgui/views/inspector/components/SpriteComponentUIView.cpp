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

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			char m_sPixelShaderName[128];
			char m_VertexShaderName[128];
			char m_TextureName[128];
			SpriteComponentUIView::SpriteComponentUIView(ImGuiWindow& a_Window, gameplay::SpriteComponent& a_SpriteComponent, gameplay::SpriteSystem& a_System) : ComponentUIView(a_Window, a_SpriteComponent, a_System), 
				m_SizeView(a_Window),
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

				memset(m_VertexShaderName, 0, sizeof(m_VertexShaderName));
				strncpy(m_VertexShaderName, m_Component.GetShader()->GetVertexShader()->GetName().c_str(), sizeof(m_VertexShaderName));
				m_VertexShaderName[sizeof(m_VertexShaderName) - 1] = '\0';

				memset(m_TextureName, 0, sizeof(m_TextureName));
				strncpy(m_TextureName, m_Component.GetTexture()->GetName().c_str(), sizeof(m_TextureName));
				m_TextureName[sizeof(m_TextureName) - 1] = '\0';

				FilePickerModal* modal = dynamic_cast<FilePickerModal*>(m_Window.GetModal((int) EDITOR_MODAL::EDITOR_MODAL_FILE_PICKER));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SPRITE_COMPONENT_TABLE_INSEPCTOR"), m_Window.GetFramePadding());
				ImGuiWindow& window = m_Window;
				gameplay::SpriteComponent& spriteComp = m_Component;
				char* pixelShaderName = m_sPixelShaderName;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Pixel Shader: ");
					},
					[&spriteComp, &buttonSize, pixelShaderName, modal]
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
						ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_PIXEL_SHADER_NAME_INPUT_INSPECTOR").c_str(), pixelShaderName, sizeof(pixelShaderName), ImGuiInputTextFlags_ReadOnly);
						ImGui::PopItemFlag();
						ImGui::SameLine();
						if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "SPRITE_COMPONENT_PIXEL_SHADER_INSPECTOR").c_str(), buttonSize))
						{
							if (modal)
							{
								modal->SetData(
									[&spriteComp](int success, gallus::resources::FileResource& resource)
									{
										if (success == 1)
										{
											spriteComp.SetShader(
												core::EDITOR_ENGINE->GetResourceAtlas().LoadShaderBind(
												spriteComp.GetTexture()->GetName(),
												core::EDITOR_ENGINE->GetResourceAtlas().LoadPixelShader(resource.GetPath().filename().generic_string()).get(),
												spriteComp.GetShader()->GetVertexShader()
											).get());

											core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
										}
									},
									std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::PixelShader }
								);
								modal->Show();
							}
						}
					});
				char* vertexShaderName = m_VertexShaderName;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Vertex Shader: ");
					},
					[&spriteComp, &buttonSize, vertexShaderName, modal]
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
						ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_VERTEX_SHADER_NAME_INPUT_INSPECTOR").c_str(), vertexShaderName, sizeof(vertexShaderName), ImGuiInputTextFlags_ReadOnly);
						ImGui::PopItemFlag();
						ImGui::SameLine();
						if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "SPRITE_COMPONENT_VERTEX_SHADER_INSPECTOR").c_str(), buttonSize))
						{
							if (modal)
							{
								modal->SetData(
									[&spriteComp](int success, gallus::resources::FileResource& resource)
									{
										if (success == 1)
										{
											spriteComp.SetShader(
												core::EDITOR_ENGINE->GetResourceAtlas().LoadShaderBind(
												spriteComp.GetTexture()->GetName(),
												spriteComp.GetShader()->GetPixelShader(),
												core::EDITOR_ENGINE->GetResourceAtlas().LoadVertexShader(resource.GetPath().filename().generic_string()).get()
											).get());

											core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
										}
									},
									std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::VertexShader }
								);
								modal->Show();
							}
						}
					});
				char* textureName = m_TextureName;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Texture: ");
					},
					[&spriteComp, &buttonSize, textureName, modal]
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
						ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_TEXTURE_NAME_INPUT_INSPECTOR").c_str(), textureName, sizeof(textureName), ImGuiInputTextFlags_ReadOnly);
						ImGui::PopItemFlag();
						ImGui::SameLine();
						if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "SPRITE_COMPONENT_TEXTURE_INSPECTOR").c_str(), buttonSize))
						{
							if (modal)
							{
								modal->SetData(
									[&spriteComp](int success, gallus::resources::FileResource& resource)
									{
										if (success == 1)
										{
											auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
											spriteComp.SetTexture(core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(resource.GetPath().filename().generic_string(), cCommandQueue).get());

											core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
										}
									},
									std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::Sprite }
								);
								modal->Show();
							}
						}
					});
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Sprite Index: ");
					},
					[&spriteComp]
					{
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						int spriteIndex = spriteComp.GetSpriteIndex();
						if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_CURRENT_SPRITE_INDEX_INSPECTOR").c_str(), &spriteIndex, 1, 0, spriteComp.GetTexture()->GetSpriteRectsSize() - 1))
						{
							spriteComp.SetSpriteIndex(spriteIndex);
						}
					});
				ColorRGBAView<DirectX::XMFLOAT4>& colorView = m_ColorView;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Color: ");
					},
					[&colorView, &spriteComp]
					{
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						colorView.SetValue(spriteComp.GetColor());
						if (colorView.Render("SPRITE_COMPONENT_COLOR_INSPECTOR"))
						{
							spriteComp.SetColor(colorView.GetValue());
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

			void SpriteComponentUIView::RenderComponentGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom)
			{
				if (!core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetDrawBounds())
				{
					return;
				}

				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImVec2 mouseScreen = ImGui::GetMousePos();

				std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

				if (!core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().HasComponent(m_Component.GetEntityID()))
				{
					return;
				}

				auto& transform = core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(m_Component.GetEntityID());

				std::array<DirectX::XMFLOAT2, 4> worldCorners =
					transform.Transform().GetWorldCorners();

				ImVec2 corners[4];
				for (int i = 0; i < 4; ++i)
				{
					corners[i] = a_vScenePos + a_vPanOffset +
						ImVec2(worldCorners[i].x * a_fZoom,
							worldCorners[i].y * a_fZoom);
				}

				// Draw bounding box polyline
				drawList->AddPolyline(corners, 4, IM_COL32(255, 255, 255, 155), true, 2.0f);
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