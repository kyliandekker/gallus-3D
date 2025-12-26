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
			{ }

			//---------------------------------------------------------------------
			AnimationWindow::~AnimationWindow()
			{
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

				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				float topPosY = ImGui::GetCursorPosY();

				bool wasDirty = m_Animation.IsDirty();
				bool wasValid = m_Animation.IsValid();
				if (!wasDirty || !wasValid)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "SAVE_ANIMATION_MODAL").c_str(), "Saves the current animation track to its file.", m_Window.GetHeaderSize(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				{
					m_Animation.Save(m_pFile->GetPath());

					m_Animation.SetIsDirty(false);

					// TODO: Reload existing resource if loaded in atlas.
					//if (core::EDITOR_ENGINE->GetResourceAtlas().HasTexture(m_pFileResource->GetPath().filename().generic_string()))
					//{
					//	core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(m_pFileResource->GetPath().filename().generic_string())->LoadMetaData();
					//}
				}
				
				if (!wasDirty || !wasValid)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				if (!wasValid)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				ImGui::SameLine();

				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_ADD_KEYFRAME), BUTTON_ID, "ADD_KEYFRAME_ANIMATION_MODAL").c_str(), "Adds a new keyframe at the currently selected frame.", m_Window.GetHeaderSize(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				{
					m_Animation.AddKeyFrame(m_iCurrentFrame);

					m_Animation.SetIsDirty(true);
				}

				if (!wasValid)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				bool wasInvalid = m_iSelectedKeyFrame < 0 || m_iSelectedKeyFrame >= m_Animation.GetKeyFrames().size();
				if (!wasValid || wasInvalid)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				ImGui::SameLine();

				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_REMOVE_KEYFRAME), BUTTON_ID, "REMOVE_KEYFRAME_ANIMATION_MODAL").c_str(), "Removes the currently selected keyframe from the track.", m_Window.GetHeaderSize(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				{
					m_Animation.RemoveKeyFrame(m_iSelectedKeyFrame);
					SetCurrentFrame(-1);

					m_Animation.SetIsDirty(true);
				}

				if (!wasValid || wasInvalid)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::SameLine();

				if (!wasValid)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				bool isLooping = m_Animation.IsLooping();
				if (ImGui::CheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_REFRESH), BUTTON_ID, "IS_LOOPING_ANIMATION_MODAL").c_str(), &isLooping, "Toggles looping for the animation track.", ImVec2(toolbarSize.y, toolbarSize.y)))
				{
					m_Animation.SetIsLooping(isLooping);

					m_Animation.SetIsDirty(true);
				}

				if (!wasValid)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::SameLine();

				int frameCount = m_Animation.GetFrameCount() == 0 ? 100 : m_Animation.GetFrameCount();

				ImGui::SetNextItemWidth(6.5f * m_Window.GetFontSize());
				ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "CURRENT_FRAME_ANIMATION_MODAL").c_str(), &m_iCurrentFrame, 1, 0, frameCount, "Frame %i");

				ImGui::SameLine();

				ImGui::SetNextItemWidth(10.5f * m_Window.GetFontSize());
				if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "FRAME_COUNT_ANIMATION_MODAL").c_str(), &frameCount, 1, 0, frameCount, "Frame Count %i"))
				{
					m_Animation.SetFrameCount(frameCount);
				}

				ImVec2 endPos = ImGui::GetCursorPos();

				ImGui::SetCursorPos(endPos);

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::EndToolbar(ImVec2(0, 0));

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));

				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "LEGEND_ANIMATION_MODAL").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					0,
					ImGuiWindowFlags_AlwaysHorizontalScrollbar
				))
				{
					ImVec2 initialPos = ImGui::GetCursorScreenPos();
					ImVec2 legendRectPos = initialPos;
					ImVec2 legendRectSize = ImVec2((m_Animation.GetFrameCount() * ANIMATION_FRAME_PIXEL_WIDTH) + LEGEND_PADDING + LEGEND_PADDING, LEGEND_HEIGHT + LEGEND_PADDING);
					ImVec2 legendPos = legendRectPos + ImVec2(LEGEND_PADDING, LEGEND_PADDING);
					ImVec2 legendSize = ImVec2(legendRectSize.x - LEGEND_PADDING, legendRectSize.y - LEGEND_PADDING);

					ImVec2 legendRectMax = ImVec2(legendRectPos.x + legendRectSize.x, legendRectPos.y + legendRectSize.y);

					if (legendRectMax.x < ImGui::GetContentRegionAvail().x)
					{
						legendRectMax.x = legendRectPos.x + ImGui::GetContentRegionAvail().x;
					}

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

					for (int i = frameMin; i <= frameCount; i += frameStep)
					{
						bool baseIndex = ((i % modFrameCount) == 0) || (i == frameCount || i == frameMin);
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
					ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "INVISIBLE_BUTTON_ANIMATION_MODAL").c_str(), legendSize);
					if (ImGui::IsMouseHoveringRect(legendPos, ImVec2(
						legendPos.x + legendSize.x,
						legendPos.y + ImGui::GetContentRegionAvail().y
					)) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
					{
						ImVec2 mousePos = ImGui::GetMousePos();
						ImVec2 relativePos = mousePos - legendPos;

						int index = std::round(relativePos.x / ANIMATION_FRAME_PIXEL_WIDTH);
						index = std::clamp(index, 0, m_Animation.GetFrameCount());
						m_iCurrentFrame = index;
					}

					ImGui::SetCursorScreenPos(ImVec2(legendPos.x, legendRectMax.y + LEGEND_PADDING));
					ImVec2 timelinePos = ImGui::GetCursorScreenPos();

					for (int i = frameMin; i <= frameCount; i += frameStep)
					{
						float px = legendPos.x + ((i - frameMin) * ANIMATION_FRAME_PIXEL_WIDTH);

						drawList->AddLine(
							ImVec2(px, timelinePos.y),
							ImVec2(px, timelinePos.y + TRACK_SIZE),
							0xFF606060,
							1.0f
						);
					}

					ImVec2 trackStartPos = ImGui::GetCursorScreenPos();


					ImVec2 startPos = ImGui::GetCursorScreenPos();

					ImRect animationTrackRect = ImRect(
						startPos,
						startPos + ImVec2(ImGui::GetContentRegionAvail().x, TRACK_SIZE)
					);

					ImGuiIO& io = ImGui::GetIO();

					ANIMATION_FRAME_PIXEL_WIDTH = ANIMATION_FRAME_PIXEL_WIDTH_DEFAULT;
					float calcLegendWidth = (m_Animation.GetFrameCount() * ANIMATION_FRAME_PIXEL_WIDTH_DEFAULT) - LEGEND_PADDING;
					if (calcLegendWidth < ImGui::GetContentRegionAvail().x)
					{
						ANIMATION_FRAME_PIXEL_WIDTH = (ImGui::GetContentRegionAvail().x - LEGEND_PADDING) / m_Animation.GetFrameCount();
					}

					int i = 0;
					for (animation::AnimationKeyFrame* keyFrame : m_Animation.GetKeyFrames())
					{
						float px = startPos.x + (keyFrame->GetFrame() * ANIMATION_FRAME_PIXEL_WIDTH);
						std::string keyFrameIcon = font::ICON_KEYFRAME;
						ImVec2 iconSize = ImGui::CalcTextSize(keyFrameIcon.c_str());
						float verticalOffset = (TRACK_SIZE - iconSize.y) / 2.0f;

						ImVec2 contentStartPos = ImVec2(px - (iconSize.x / 2), startPos.y + verticalOffset);
						ImVec2 contentEndPos = ImVec2(contentStartPos.x + iconSize.x, contentStartPos.y + iconSize.y);

						ImGui::SetCursorScreenPos(contentStartPos);

						bool bSelected = m_iSelectedKeyFrame == i;

						if (bSelected)
						{
							ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent), keyFrameIcon.c_str());
						}
						else
						{
							ImGui::Text(keyFrameIcon.c_str());
						}

						bool bHovered = ImGui::IsMouseHoveringRect(contentStartPos, contentEndPos);
						bool bMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
						bool bMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
						bool bMouseDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

						static bool s_bDragging = false;
						static animation::AnimationKeyFrame* s_pDraggedKeyFrame = nullptr;

						// Start drag
						if (bHovered && bMouseClicked)
						{
							SetCurrentFrame(i);

							s_bDragging = true;
							s_pDraggedKeyFrame = keyFrame;
						}

						// Update drag
						if (s_bDragging && s_pDraggedKeyFrame == keyFrame && bMouseDragging)
						{
							ImVec2 mousePos = ImGui::GetMousePos();
							ImVec2 s_vRelativePos = mousePos - startPos;
							int frame = std::round(s_vRelativePos.x / ANIMATION_FRAME_PIXEL_WIDTH);
							keyFrame->SetFrame(std::clamp(frame, 0, m_Animation.GetFrameCount()));

							keyFrame->GetAnimation()->SetIsDirty(true);
						}

						// End drag
						if (!bMouseDown && s_bDragging && s_pDraggedKeyFrame == keyFrame)
						{
							s_bDragging = false;
							s_pDraggedKeyFrame = nullptr;
						}

						i++;
					}

					ImGui::SetCursorScreenPos(ImVec2(startPos.x, animationTrackRect.Max.y));

					drawList->AddLine(
						ImVec2(legendRectPos.x, trackStartPos.y),
						ImVec2(legendRectMax.x, trackStartPos.y),
						0xFFFFFFFF,
						1.0f
					);

					drawList->AddLine(
						ImVec2(legendRectPos.x, trackStartPos.y + TRACK_SIZE),
						ImVec2(legendRectMax.x, trackStartPos.y + TRACK_SIZE),
						0xFFFFFFFF,
						1.0f
					);
				}

				ImGui::EndChild();
			}

			void AnimationWindow::SetData(gallus::resources::FileResource& a_File)
			{
				m_pFile = &a_File;

				fs::path animationPath = a_File.GetPath();
				if (m_Animation.LoadByPath(animationPath))
				{
					for (auto& keyFrame : m_Animation.GetKeyFrames())
					{
						m_KeyFrameSelectables.emplace_back(m_Window, *keyFrame, m_Animation);
					}
				}
				
				m_iCurrentFrame = 0;
				m_iSelectedKeyFrame = -1;

			}

			void AnimationWindow::SetCurrentFrame(int a_iIndex)
			{
				int index = std::clamp(a_iIndex, -1, m_Animation.GetFrameCount());
				if (index == -1)
				{
					core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr);
				}
				else
				{
					core::EDITOR_ENGINE->GetEditor().SetSelectable(&m_KeyFrameSelectables[a_iIndex]);
				}

				m_iSelectedKeyFrame = a_iIndex;
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE