#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "AnimationWindow.h"

#include <imgui/imgui_helpers.h>
#include <imgui/ImSequencer.h>
#include <imgui/imgui_internal.h> // for ImClamp

#include "graphics/imgui/font_icon.h"

#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/EditorSelectable.h"

#include "graphics/dx12/Texture.h"

#include "animation/AnimationTrack.h"
#include "animation/SpriteAnimationKeyFrame.h"

#include "gameplay/Game.h"
#include <editor/graphics/imgui/modals/FilePickerModal.h>

//---------------------------------------------------------------------
namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			constexpr float ANIMATION_FRAME_PIXEL_WIDTH = 25.0f;
			constexpr float LEGEND_PADDING = 25.0f;
			constexpr float LEGEND_HEIGHT = 70.0f;
			constexpr float TRACK_SIZE = 75;
			int m_iSelectedKeyFrame = -1;

			class SpriteAnimationKeyFrameUIView : public EditorSelectable
			{
			public:
				SpriteAnimationKeyFrameUIView(ImGuiWindow& a_Window, animation::SpriteAnimationKeyFrame& a_KeyFrame) : EditorSelectable(),
					m_KeyFrame(a_KeyFrame)
				{

				}

				void RenderEditorSelectable() override
				{
				}

				animation::SpriteAnimationKeyFrame& GetKeyFrame()
				{
					return m_KeyFrame;
				}

				const animation::SpriteAnimationKeyFrame& GetKeyFrame() const
				{
					return m_KeyFrame;
				}
			private:
				animation::SpriteAnimationKeyFrame& m_KeyFrame;
			};

			class SpriteAnimationKeyFrameInspectorUIView : public InspectorView
			{
			public:
				SpriteAnimationKeyFrameInspectorUIView(ImGuiWindow& a_Window, SpriteAnimationKeyFrameUIView& a_SpriteAnimationKeyFrameUIView) : InspectorView(a_Window),
					m_SpriteAnimationKeyFrameUIView(a_SpriteAnimationKeyFrameUIView)
				{
					m_bShowDelete = true;
					m_bShowPreview = true;
				}

				std::string GetName() const override
				{
					return "Sprite Animation Key Frame";
				}

				std::string GetIcon() const override
				{
					return font::ICON_KEYFRAME;
				}

				void Render() override
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
					FilePickerModal* modal = dynamic_cast<FilePickerModal*>(m_Window.GetModal((int)EDITOR_MODAL::EDITOR_MODAL_FILE_PICKER));

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);

					char* textureName = m_TextureName;
					ImGui::KeyValue([&window]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(window.GetBoldFont(), "Texture: ");
						},
						[this, &buttonSize, textureName, modal]
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
									modal->Show();
								}
							}
						});
					ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}

				void RenderPreview() override
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
					ImGui::Image((ImTextureID)m_SpriteAnimationKeyFrameUIView.GetKeyFrame().GetTexture()->GetGPUHandle().ptr, ImVec2(drawW, drawH), uv0, uv1);

					// Draw border
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					draw_list->AddRect(image_pos, ImVec2(image_pos.x + drawW, image_pos.y + drawH),
						ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border]));
				}
			private:
				SpriteAnimationKeyFrameUIView& m_SpriteAnimationKeyFrameUIView;
				char m_TextureName[128];
			};

			class AnimationTrackUIView : public ImGuiUIView
			{
			public:
				AnimationTrackUIView(ImGuiWindow& a_Window, animation::AnimationTrack<animation::SpriteAnimationKeyFrame>& a_AnimationTrack) : ImGuiUIView(a_Window), a_AnimationTrack(a_AnimationTrack)
				{
					for (size_t i = 0; i < a_AnimationTrack.GetKeyFrames().size(); i++)
					{
						m_aKeyFrames.push_back(SpriteAnimationKeyFrameUIView(a_Window, a_AnimationTrack.GetKeyFrames()[i]));
					}
				}

				void Render() override
				{
					ImDrawList* drawList = ImGui::GetWindowDrawList();
					ImVec2 startPos = ImGui::GetCursorScreenPos();

					size_t i = 0;
					for (SpriteAnimationKeyFrameUIView& keyFrameUIView : m_aKeyFrames)
					{
						int frame = keyFrameUIView.GetKeyFrame().GetFrame();

						float px = startPos.x + (frame * ANIMATION_FRAME_PIXEL_WIDTH);

						std::string keyFrameIcon = font::ICON_KEYFRAME;
						ImVec2 iconSize = ImGui::CalcTextSize(keyFrameIcon.c_str());

						float verticalOffset = (TRACK_SIZE - iconSize.y) / 2.0f;

						ImVec2 contentStartPos = ImVec2(px - (iconSize.x / 2), startPos.y + verticalOffset);
						ImGui::SetCursorScreenPos(contentStartPos);
						if (m_iSelectedKeyFrame == i)
						{
							ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent), keyFrameIcon.c_str());
						}
						else
						{
							ImGui::Text(keyFrameIcon.c_str());
						}

						if (ImGui::IsMouseHoveringRect(contentStartPos, ImVec2(
							contentStartPos.x + iconSize.x,
							contentStartPos.y + iconSize.y
						)) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
						{
							m_iSelectedKeyFrame = i;

							core::EDITOR_ENGINE->GetEditor().SetSelectable(&m_aKeyFrames[i], new SpriteAnimationKeyFrameInspectorUIView(m_Window, m_aKeyFrames[i]));
						}
						i++;
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + TRACK_SIZE);
				}

				std::vector<SpriteAnimationKeyFrameUIView> m_aKeyFrames;
				animation::AnimationTrack<animation::SpriteAnimationKeyFrame>& a_AnimationTrack;
			};

			animation::AnimationTrack<animation::SpriteAnimationKeyFrame> m_SpriteAnimatorTrack;
			AnimationTrackUIView* m_AnimationTrackUIView = nullptr;

			AnimationWindow::AnimationWindow(ImGuiWindow& a_Window)
				: BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse,
					std::string(font::ICON_ANIMATION) + " Animation", "Animation")
			{
				m_SpriteAnimatorTrack.GetKeyFrames().push_back(animation::SpriteAnimationKeyFrame(12, 0));
				m_SpriteAnimatorTrack.GetKeyFrames().push_back(animation::SpriteAnimationKeyFrame(25, 1));
				m_AnimationTrackUIView = new AnimationTrackUIView(a_Window, m_SpriteAnimatorTrack);
			}

			AnimationWindow::~AnimationWindow() {}

			void AnimationWindow::Update()
			{
				if (gameplay::GAME.IsStarted() && !gameplay::GAME.IsPaused())
				{
					return;
				}

				BaseWindow::Update();
			}

			int m_iCurrentFrame = 0;
			void AnimationWindow::Render()
			{
				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				ImGui::SetCursorPos(
					ImVec2(
						ImGui::GetCursorPos().x + m_Window.GetFramePadding().x,
						ImGui::GetCursorPos().y + m_Window.GetFramePadding().y
					)
				);

				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "BOX_ANIMATION").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders,
					ImGuiWindowFlags_AlwaysHorizontalScrollbar
				))
				{
					int frameMax = 300;

					ImVec2 initialPos = ImGui::GetCursorScreenPos();
					ImVec2 legendRectPos = initialPos + ImVec2(200, 0);
					ImVec2 legendRectSize = ImVec2((frameMax * ANIMATION_FRAME_PIXEL_WIDTH) + LEGEND_PADDING + LEGEND_PADDING + LEGEND_PADDING, LEGEND_HEIGHT + LEGEND_PADDING);
					ImVec2 legendPos = legendRectPos + ImVec2(LEGEND_PADDING, LEGEND_PADDING);
					ImVec2 legendSize = ImVec2(legendRectSize.x - LEGEND_PADDING, legendRectSize.y - LEGEND_PADDING);

					ImVec2 legendRectMax = ImVec2(legendRectPos.x + legendRectSize.x, legendRectPos.y + legendRectSize.y);

					ImDrawList* drawList = ImGui::GetWindowDrawList();

					// --- Background ---
					drawList->AddRectFilled(
						legendRectPos,
						legendRectMax,
						0xFF3D3837,
						0.0f
					);

					// --- Frame setup ---
					int frameMin = 0;
					float visibleWidth = legendSize.x;

					// Modulo spacing for major ticks
					int modFrameCount = 10;
					int frameStep = 1;

					int halfModFrameCount = modFrameCount / 2;

					// --- Draw ticks ---
					for (int i = frameMin; i <= frameMax; i += frameStep)
					{
						bool baseIndex = ((i % modFrameCount) == 0) || (i == frameMax || i == frameMin);
						bool halfIndex = (i % halfModFrameCount) == 0;

						float px = legendPos.x + ((i - frameMin) * ANIMATION_FRAME_PIXEL_WIDTH);
						int tiretStart = baseIndex ? 4 : (halfIndex ? 10 : 14);
						int tiretEnd = (int)legendSize.y - 4;

						drawList->AddLine(
							ImVec2(px, legendPos.y + (float)tiretStart),
							ImVec2(px, legendPos.y + (float)tiretEnd),
							0xFF606060,
							1.0f
						);

						if (baseIndex)
						{
							char tmp[32];
							ImFormatString(tmp, IM_ARRAYSIZE(tmp), "%d", i);
							ImVec2 textSize = ImGui::CalcTextSize(tmp);
							drawList->AddText(
								ImVec2(px - (textSize.x / 2), legendPos.y - textSize.y),
								0xFFBBBBBB,
								tmp
							);
						}
					}

					float selectedFramePos = legendPos.x + ((m_iCurrentFrame - frameMin) * ANIMATION_FRAME_PIXEL_WIDTH);

					drawList->AddLine(
						ImVec2(selectedFramePos, legendPos.y),
						ImVec2(selectedFramePos, legendPos.y + (float)ImGui::GetContentRegionAvail().y),
						0xA02A2AFF,
						1.0f
					);

					ImGui::SetCursorScreenPos(legendPos);
					ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "INVISIBLE_BUTTON_ANIMATION").c_str(), legendSize);
					if (ImGui::IsMouseHoveringRect(legendPos, ImVec2(
						legendPos.x + legendSize.x,
						legendPos.y + ImGui::GetContentRegionAvail().y
					)) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
					{
						ImVec2 mousePos = ImGui::GetMousePos();
						ImVec2 relativePos = mousePos - legendPos;

						m_iCurrentFrame = std::round(relativePos.x / ANIMATION_FRAME_PIXEL_WIDTH);
					}

					ImGui::SetCursorScreenPos(initialPos + m_Window.GetFramePadding());
					ImGui::SetNextItemWidth(100);
					ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "CURRENT_FRAME_ANIMATION").c_str(), &m_iCurrentFrame, 1, 0, frameMax);

					ImGui::SetCursorScreenPos(ImVec2(legendPos.x, legendRectMax.y));

					m_AnimationTrackUIView->Render();
				}

				ImGui::EndChild();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE

