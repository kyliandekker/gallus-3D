#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "AnimationWindow.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_toggle.h>
#include <imgui/imgui_internal.h>

// graphics
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// animation
#include "animation/AnimationKeyFrameSpriteComponent.h"
#include "animation/AnimationKeyFrameEventComponent.h"

// utils
#include "utils/string_extensions.h"

// gameplay
#include "gameplay/Game.h"

// editor
#include "editor/core/EditorEngine.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// AnimationWindow
			//---------------------------------------------------------------------
			AnimationWindow::AnimationWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_ANIMATION) + " Animation", "Animation")
			{}

			//---------------------------------------------------------------------
			AnimationWindow::~AnimationWindow()
			{
			}

			//---------------------------------------------------------------------
			bool AnimationWindow::Initialize()
			{
				PopulateToolbar();

				return BaseWindow::Initialize();
			}

			//---------------------------------------------------------------------
			void AnimationWindow::PopulateToolbar()
			{
				ImVec2 toolbarSize = ImVec2(0, m_Window.GetHeaderSize().y);
				m_Toolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_ANIMATION"), toolbarSize);

				// Add keyframe button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,

					[this]()
					{
						if (ImGui::IconButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_ADD_KEYFRAME), BUTTON_ID, "ADD_KEYFRAME_ANIMATION_MODAL").c_str(), "Adds a new keyframe at the currently selected frame.", m_Window.GetHeaderSize(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
						{
							AddKeyFrame(m_iSelectedKeyFrame);
						}
					}
				));

				// Remove keyframe button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,

					[this]()
					{
						if (ImGui::IconButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_REMOVE_KEYFRAME), BUTTON_ID, "REMOVE_KEYFRAME_ANIMATION_MODAL").c_str(), "Removes the currently selected keyframe from the track.", m_Window.GetHeaderSize(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
						{
							RemoveKeyFrame(m_iSelectedKeyFrame);
						}
					},
					[this]()
					{
						return m_iSelectedKeyFrame == -1;
					}
				));

				// Looping button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,

					[this]()
					{
						bool isLooping = m_Animation.IsLooping();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_REFRESH), BUTTON_ID, "IS_LOOPING_ANIMATION_MODAL").c_str(), &isLooping, "Toggles looping for the animation track.", m_Window.GetHeaderSize()))
						{
							m_Animation.SetIsLooping(isLooping);

							m_Animation.SetIsDirty(true);
						}
					}
				));
			}

			//---------------------------------------------------------------------
			void AnimationWindow::DrawToolbar()
			{
				// Start toolbar.
				m_Toolbar.StartToolbar();

				// Render toolbar.
				m_Toolbar.Render();

				// End toolbar.
				m_Toolbar.EndToolbar();
			}

			//---------------------------------------------------------------------
			void AnimationWindow::RemoveKeyFrame(size_t a_iIndex)
			{
				m_Animation.RemoveKeyFrame(m_iSelectedKeyFrame);
				SetCurrentFrame(-1);

				m_Animation.SetIsDirty(true);
			}

			//---------------------------------------------------------------------
			void AnimationWindow::AddKeyFrame(size_t a_iIndex)
			{
				m_Animation.AddKeyFrame(m_iSelectedFrame);
				m_Animation.SetIsDirty(true);
			}

			//---------------------------------------------------------------------
			void AnimationWindow::Update()
			{
				if (core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetFullScreenPlayMode())
				{
					return;
				}
				BaseWindow::Update();
			}

			//---------------------------------------------------------------------
			void AnimationWindow::Render()
			{
				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				DrawToolbar();

				ImGui::SetCursorPos({
					ImGui::GetCursorPos().x + m_Window.GetFramePadding().x,
					ImGui::GetCursorPos().y + m_Window.GetFramePadding().y,
				});

				int frameCount = m_Animation.GetFrameCount() == 0 ? 100 : m_Animation.GetFrameCount();
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "MAIN_ANIMATION").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders,
					ImGuiWindowFlags_AlwaysHorizontalScrollbar
				))
				{
					ImVec2 legendPadding = m_Window.GetFramePadding();
					ImVec2 initialPos = ImGui::GetCursorScreenPos();
					
					ImRect legendRect = {
						initialPos,
						initialPos + ImVec2((frameCount * ANIMATION_FRAME_PIXEL_WIDTH) + (legendPadding.x * 2), LEGEND_HEIGHT)
					};

					ImRect legendBackground = legendRect;
					if (legendBackground.GetSize().x < ImGui::GetContentRegionAvail().x)
					{
						legendBackground.Max.x = legendBackground.Min.x + ImGui::GetContentRegionAvail().x;
					}

					ImDrawList* drawList = ImGui::GetWindowDrawList();

					ImVec2 timeLineStart = {
						legendRect.Min.x + legendPadding.x,
						legendRect.Min.y
					};

					// --- Frame setup ---
					int frameMin = 0;
					float fullWidth = legendRect.GetSize().x;

					// Modulo spacing for major ticks
					int modFrameCount = 5;
					int frameStep = 1;

					// --- Legend with ticks ---
					if (ImGui::BeginChild(
						ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "LEGEND_TICKS_ANIMATION").c_str(),
						legendRect.GetSize()
					))
					{
						// --- Background ---
						drawList->AddRectFilled(
							legendBackground.Min,
							legendBackground.Max,
							0xFF3D3837,
							0.0f
						);

						// --- Legend ticks ---
						{
							int halfModFrameCount = modFrameCount / 2;

							float majorLineHeight = LEGEND_HEIGHT - (m_Window.GetFontSize() + legendPadding.y);
							float halfLineHeight = majorLineHeight / 2;
							float normalLineHeight = halfLineHeight / 2;
							int lineEnd = legendRect.Max.y - 4;

							for (int i = frameMin; i <= frameCount; i += frameStep)
							{
								bool baseIndex = ((i % modFrameCount) == 0) || (i == frameCount || i == frameMin);
								bool halfIndex = (i % halfModFrameCount) == 0;

								float px = timeLineStart.x + ((i - frameMin) * ANIMATION_FRAME_PIXEL_WIDTH);

								int lineStart = lineEnd;
								lineStart -= baseIndex ? majorLineHeight : (halfIndex ? halfLineHeight : normalLineHeight);
								ImRect lineRect = {
									ImVec2(px, lineEnd),
									ImVec2(px, lineStart),
								};

								drawList->AddLine(
									ImVec2(px, lineStart),
									ImVec2(px, lineEnd),
									0xFF606060,
									1.0f
								);

								if (baseIndex)
								{
									char tmp[32];
									ImFormatString(tmp, IM_ARRAYSIZE(tmp), "%d", i);
									ImVec2 textSize = ImGui::CalcTextSize(tmp);

									ImVec2 textPos = {
										px - (textSize.x / 2),
										lineStart - textSize.y
									};

									drawList->AddText(
										textPos,
										0xFFBBBBBB,
										tmp
									);
								}
							}
						}
					}
					ImGui::EndChild();

					ImRect keyFrameTimelineRect = {
						{ legendRect.Min.x, ImGui::GetCursorScreenPos().y },
						{ legendRect.Max.x, legendRect.Max.y + ImGui::GetContentRegionAvail().y }
					};

					static bool s_bDragging = false;
					static animation::AnimationKeyFrame* s_pDraggedKeyFrame = nullptr;
					static int s_iDraggedKeyFrameIndex = -1;
					bool bMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);

					// --- Key Frame timeline ---
					if (ImGui::BeginChild(
						ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "KEY_FRAME_TIMELINE_ANIMATION").c_str(),
						keyFrameTimelineRect.GetSize()
					))
					{
						float keyFrameY = keyFrameTimelineRect.Min.y - ImGui::GetScrollY();

						std::string keyFrameIcon = font::ICON_KEYFRAME;

						int i = 0;
						for (animation::AnimationKeyFrame* keyFrame : m_Animation.GetKeyFrames())
						{
							ImVec2 iconSize = ImGui::CalcTextSize(keyFrameIcon.c_str());
							float px = timeLineStart.x + (keyFrame->GetFrame() * ANIMATION_FRAME_PIXEL_WIDTH);
							px -= iconSize.x / 2;

							ImRect textRect = {
								{ px, keyFrameY },
								{ px + iconSize.x, keyFrameY + iconSize.y },
							};

							bool isSelected = m_iSelectedKeyFrame == i;
							ImU32 col = isSelected ? ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)) : 0xFFBBBBBB;

							drawList->AddText(
								textRect.Min,
								col,
								keyFrameIcon.c_str()
							);

							bool bHovered = ImGui::IsMouseHoveringRect(textRect.Min, textRect.Max, false);
							bool bMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
							bool bMouseDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

							// Start drag
							if (bHovered && bMouseClicked)
							{
								SetCurrentFrame(i);

								s_bDragging = true;
								s_pDraggedKeyFrame = keyFrame;
								s_iDraggedKeyFrameIndex = i;
							}

							// End drag
							if (!bMouseDown && s_bDragging && s_pDraggedKeyFrame == keyFrame)
							{
								s_bDragging = false;
								s_pDraggedKeyFrame = nullptr;
								s_iDraggedKeyFrameIndex = -1;
							}

							i++;
						}
					}
					ImGui::EndChild();
					
					// --- Tracker ---
					float selectedFramePos = timeLineStart.x + ((m_iSelectedFrame - frameMin) * ANIMATION_FRAME_PIXEL_WIDTH);

					drawList->AddLine(
						ImVec2(selectedFramePos, legendRect.Min.y),
						ImVec2(selectedFramePos, keyFrameTimelineRect.Max.y),
						0xA02A2AFF,
						1.0f
					);

					// Get the scroll offset of the child window that contains the timeline
					float scrollX = ImGui::GetScrollX();

					if (ImGui::IsMouseHoveringRect(legendRect.Min, keyFrameTimelineRect.Max) && bMouseDown)
					{
						ImVec2 mousePos = ImGui::GetMousePos();

						// Get the scroll offset of the child window that contains the timeline
						ImVec2 scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());

						// Compute relative position in timeline space
						ImVec2 relativePos = mousePos - legendRect.Min + scroll;

						int index = std::floor(relativePos.x / ANIMATION_FRAME_PIXEL_WIDTH);
						index = std::clamp(index, 0, frameCount);

						m_iSelectedFrame = index;

						if (s_bDragging && s_pDraggedKeyFrame)
						{
							if (m_Animation.GetKeyFrameAtFrame(m_iSelectedFrame) == -1)
							{
								s_pDraggedKeyFrame->SetFrame(m_iSelectedFrame);
								s_pDraggedKeyFrame->GetAnimation()->SetIsDirty(true);
							}
						}
					}
				}

				ImGui::EndChild();

				return;


				//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				//ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				//float topPosY = ImGui::GetCursorPosY();

				//bool wasDirty = m_Animation.IsDirty();
				//bool wasValid = m_Animation.IsValid();
				//if (!wasDirty || !wasValid)
				//{
				//	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				//	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				//}

				//if (ImGui::IconButton(
				//	ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "SAVE_ANIMATION_MODAL").c_str(), "Saves the current animation track to its file.", m_Window.GetHeaderSize(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				//{
				//	m_Animation.Save(m_pFile->GetPath());

				//	m_Animation.SetIsDirty(false);

				//	// TODO: Reload existing resource if loaded in atlas.
				//	//if (core::EDITOR_ENGINE->GetResourceAtlas().HasTexture(m_pFileResource->GetPath().filename().generic_string()))
				//	//{
				//	//	core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(m_pFileResource->GetPath().filename().generic_string())->LoadMetaData();
				//	//}
				//}
				//
				//if (!wasDirty || !wasValid)
				//{
				//	ImGui::PopItemFlag();
				//	ImGui::PopStyleVar();
				//}

				//if (!wasValid)
				//{
				//	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				//	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				//}

				//ImGui::SameLine();


				//if (!wasValid)
				//{
				//	ImGui::PopItemFlag();
				//	ImGui::PopStyleVar();
				//}

				//bool wasInvalid = m_iSelectedKeyFrame < 0 || m_iSelectedKeyFrame >= m_Animation.GetKeyFrames().size();
				//if (!wasValid || wasInvalid)
				//{
				//	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				//	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				//}

				//ImGui::SameLine();

				//if (!wasValid || wasInvalid)
				//{
				//	ImGui::PopItemFlag();
				//	ImGui::PopStyleVar();
				//}

				//ImGui::SameLine();

				//if (!wasValid)
				//{
				//	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				//	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				//}


				//if (!wasValid)
				//{
				//	ImGui::PopItemFlag();
				//	ImGui::PopStyleVar();
				//}

				//ImGui::SameLine();

				//int frameCount = m_Animation.GetFrameCount() == 0 ? 100 : m_Animation.GetFrameCount();

				//ImGui::SetNextItemWidth(6.5f * m_Window.GetFontSize());
				//ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "CURRENT_FRAME_ANIMATION_MODAL").c_str(), &m_iCurrentFrame, 1, 0, frameCount, "Frame %i");

				//ImGui::SameLine();

				//ImGui::SetNextItemWidth(10.5f * m_Window.GetFontSize());
				//if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "FRAME_COUNT_ANIMATION_MODAL").c_str(), &frameCount, 1, 0, frameCount, "Frame Count %i"))
				//{
				//	m_Animation.SetFrameCount(frameCount);
				//}

				//ImVec2 endPos = ImGui::GetCursorPos();

				//ImGui::SetCursorPos(endPos);

				//ImGui::PopStyleVar();
				//ImGui::PopStyleVar();

				////ImGui::EndToolbar(ImVec2(0, 0));

				//ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));

				
			}

			//---------------------------------------------------------------------
			void AnimationWindow::SetData(gallus::resources::FileResource& a_File)
			{
				m_pFile = &a_File;

				fs::path animationPath = a_File.GetPath();
				if (!m_Animation.LoadByPath(animationPath))
				{
					return;
				}
				
				m_iSelectedFrame = 0;
				m_iSelectedKeyFrame = -1;
			}

			//---------------------------------------------------------------------
			void AnimationWindow::SetCurrentFrame(int a_iIndex)
			{
				int index = std::clamp(a_iIndex, -1, m_Animation.GetFrameCount());
				if (index == -1)
				{
					core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr);
				}
				else
				{
					AnimationKeyFrameEditorSelectable* previous = m_KeyFrameSelectable;
					m_KeyFrameSelectable = new AnimationKeyFrameEditorSelectable(m_Window, *m_Animation.GetKeyFrames()[a_iIndex], m_Animation);
					core::EDITOR_ENGINE->GetEditor().SetSelectable(m_KeyFrameSelectable);
					if (previous)
					{
						delete previous;
					}
				}

				m_iSelectedKeyFrame = a_iIndex;
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE