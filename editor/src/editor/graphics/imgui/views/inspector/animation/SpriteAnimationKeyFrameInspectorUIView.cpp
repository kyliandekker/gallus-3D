#include "SpriteAnimationKeyFrameInspectorUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

#include "graphics/imgui/font_icon.h"

#include "editor/graphics/imgui/views/inspector/animation/SpriteAnimationKeyFrameUIView.h"

#include "graphics/dx12/Texture.h"

#include "animation/AnimationTrack.h"
#include "animation/SpriteAnimationKeyFrame.h"

#include "gameplay/Game.h"
#include "gameplay/systems/SpriteSystem.h"

#include "editor/graphics/imgui/modals/FilePickerModal.h"
#include "editor/core/EditorEngine.h"

#include "editor/graphics/imgui/EditorWindowsConfig.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string SpriteAnimationKeyFrameInspectorUIView::GetIcon() const
			{
				return font::ICON_KEYFRAME;
			}

			void SpriteAnimationKeyFrameInspectorUIView::Render()
			{
				memset(m_TextureName, 0, sizeof(m_TextureName));
				if (m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture())
				{
					strncpy(m_TextureName, m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->GetName().c_str(), sizeof(m_TextureName));
					m_TextureName[sizeof(m_TextureName) - 1] = '\0';
				}

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SPRITE_ANIMATION_KEY_FRAME_TABLE_INSPECTOR"), m_Window.GetFramePadding());
				ImGuiWindow& window = m_Window;
				ImGui::KeyValue([&window, this]
				{
					ImGui::AlignTextToFramePadding();
					ImGui::DisplayHeader(window.GetBoldFont(), "Sprite Index: ");
				},
					[this]
				{
					if (!m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture())
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					}
					int maxIndex = m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture() ? m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->GetSpriteRectsSize() : 0;
					int spriteIndex = m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetSpriteIndex();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_ANIMATION_KEY_FRAME_SPRITE_INDEX_INSPECTOR").c_str(), &spriteIndex, 1, 0, maxIndex))
					{
						m_SpriteAnimationKeyFrameUIView.GetKeyFrame().SetSpriteIndex(spriteIndex);
					}
					if (!m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture())
					{
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
				});
				FilePickerModal& filePickerModal = m_Window.GetWindowsConfig<EditorWindowsConfig>().GetFilePickerModal();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);

				char* textureName = m_TextureName;
				ImGui::KeyValue([&window]
				{
					ImGui::AlignTextToFramePadding();
					ImGui::DisplayHeader(window.GetBoldFont(), "Texture: ");
				},
					[this, &buttonSize, textureName, &filePickerModal]
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
					ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_TEXTURE_NAME_INPUT_INSPECTOR").c_str(), textureName, sizeof(textureName), ImGuiInputTextFlags_ReadOnly);
					ImGui::PopItemFlag();
					ImGui::SameLine();
					if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "SPRITE_COMPONENT_TEXTURE_INSPECTOR").c_str(), buttonSize))
					{
						filePickerModal.SetData(
							[&](int success, gallus::resources::FileResource& resource)
							{
								if (success == 1)
								{
									auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
									m_SpriteAnimationKeyFrameUIView.GetKeyFrame().SetTexure(core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(resource.GetPath().filename().generic_string(), cCommandQueue).get());

									core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
								}
							},
							std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::Sprite }
						);
						filePickerModal.Show();
					}
				});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			void SpriteAnimationKeyFrameInspectorUIView::RenderPreview()
			{
				if (!m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture() || !m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->CanBeDrawn())
				{
					return;
				}

				// Sprite dimensions
				float spriteW = 0.0f;
				float spriteH = 0.0f;
				ImVec2 uv0, uv1;

				// Get TextureMetaData
				ImVec2 texturePos = ImGui::GetCursorPos();
				if (m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->GetTextureType() == graphics::dx12::TextureType::Texture2D)
				{
					// Full texture
					spriteW = static_cast<float>(m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->GetResourceDesc().Width);
					spriteH = static_cast<float>(m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->GetResourceDesc().Height);
					uv0 = { 0.0f, 0.0f };
					uv1 = { 1.0f, 1.0f };
				}
				else
				{
					const auto& sprite = m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->GetSpriteRect(m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetSpriteIndex());
					spriteW = static_cast<float>(sprite.width);
					spriteH = static_cast<float>(sprite.height);

					const float texWidth = static_cast<float>(m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->GetResourceDesc().Width);
					const float texHeight = static_cast<float>(m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->GetResourceDesc().Height);

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
				ImGui::Image((ImTextureID) m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->GetGPUHandle().ptr, ImVec2(drawW, drawH), uv0, uv1);

				// Draw border
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->AddRect(image_pos, ImVec2(image_pos.x + drawW, image_pos.y + drawH),
					ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border]));
			}
		}
	}
}