#include "AnimationKeyFrameSpriteComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

#include "graphics/dx12/Texture.h"
#include "editor/graphics/imgui/EditorWindowsConfig.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			void AnimationKeyFrameSpriteComponentUIView::RenderInner()
			{
				memset(m_sTextureName, 0, sizeof(m_sTextureName));
				if (m_Component.GetTexture())
				{
					strncpy(m_sTextureName, m_Component.GetTexture()->GetName().c_str(), sizeof(m_sTextureName));
					m_sTextureName[sizeof(m_sTextureName) - 1] = '\0';
				}

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SPRITE_ANIMATION_KEY_FRAME_TABLE_INSPECTOR"), m_Window.GetFramePadding());
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Sprite Index: ");
					},
					[this]
					{
						if (!m_Component.GetTexture())
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						}
						int maxIndex = m_Component.GetTexture() ? m_Component.GetTexture()->GetSpriteRectsSize() : 0;
						int spriteIndex = m_Component.GetSpriteIndex();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_ANIMATION_KEY_FRAME_SPRITE_INDEX_INSPECTOR").c_str(), &spriteIndex, 1, 0, maxIndex))
						{
							m_Component.SetSpriteIndex(spriteIndex);
						}
						if (!m_Component.GetTexture())
						{
							ImGui::PopItemFlag();
							ImGui::PopStyleVar();
						}
					});
				FilePickerModal& filePickerModal = m_Window.GetWindowsConfig<EditorWindowsConfig>().GetFilePickerModal();
				AnimationEditorModal& animationEditorModal = m_Window.GetWindowsConfig<EditorWindowsConfig>().GetAnimationEditorModal();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);

				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Texture: ");
					},
					[this, &buttonSize, &filePickerModal, &animationEditorModal]
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
						ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_TEXTURE_NAME_INPUT_INSPECTOR").c_str(), m_sTextureName, sizeof(m_sTextureName), ImGuiInputTextFlags_ReadOnly);
						ImGui::PopItemFlag();
						ImGui::SameLine();
						if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "SPRITE_COMPONENT_TEXTURE_INSPECTOR").c_str(), buttonSize))
						{
							filePickerModal.SetData(
								[this, &animationEditorModal](int success, gallus::resources::FileResource& resource)
								{
									if (success == 1)
									{
										auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
										m_Component.SetTexture(core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(resource.GetPath().filename().generic_string(), cCommandQueue).get());

										core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
									}
									animationEditorModal.Show();
								},
								std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::Sprite }
							);
							animationEditorModal.Hide();
							filePickerModal.Show();
						}
					});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

				if (!m_Component.GetTexture() || !m_Component.GetTexture()->CanBeDrawn())
				{
					return;
				}

				ImGui::Unindent();

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
				ImGui::Image((ImTextureID)m_Component.GetTexture()->GetGPUHandle().ptr, ImVec2(drawW, drawH), uv0, uv1);

				// Draw border
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->AddRect(image_pos, ImVec2(image_pos.x + drawW, image_pos.y + drawH),
					ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border]));

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);

				ImGui::Indent();
			}

			std::string AnimationKeyFrameSpriteComponentUIView::GetName() const
			{
				return m_Component.GetName();
			}
		}
	}
}