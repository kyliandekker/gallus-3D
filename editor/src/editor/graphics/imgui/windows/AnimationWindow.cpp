#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "AnimationWindow.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_toggle.h>
#include <imgui/imgui_internal.h>

// utils includes
#include "utils/string_extensions.h"

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// editor includes
#include "editor/core/EditorEngine.h"

// game includes
#include "gameplay/Game.h"

#include "animation/AnimationKeyFrameSpriteComponent.h"
#include "animation/AnimationKeyFrameEventComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// AnimationWindow
			//---------------------------------------------------------------------
			AnimationWindow::AnimationWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_ANIMATION) + " Animation", "Animation"),
				m_AnimationTrackUIView(a_Window)
			{ }

			//---------------------------------------------------------------------
			AnimationWindow::~AnimationWindow()
			{
			}

			//---------------------------------------------------------------------
			void AnimationWindow::Update()
			{
				if (gameplay::GAME.IsStarted() && !gameplay::GAME.IsPaused())
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


				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "SAVE_ANIMATION_MODAL").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				{
					m_AnimationTrack.Save(m_pFile->GetPath());
				}

				ImGui::SameLine();

				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_ADD_KEYFRAME), BUTTON_ID, "ADD_KEYFRAME_ANIMATION_MODAL").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				{
					m_AnimationTrack.AddKeyFrame(m_iCurrentFrame);
				}

				ImGui::SameLine();

				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_REMOVE_KEYFRAME), BUTTON_ID, "REMOVE_KEYFRAME_ANIMATION_MODAL").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				{
					m_AnimationTrack.RemoveKeyFrame(m_AnimationTrackUIView.GetSelectedKeyFrame());
					m_AnimationTrackUIView.SetSelectedKeyFrame(-1);
				}

				ImGui::SameLine();

				bool isLooping = m_AnimationTrack.IsLooping();

				if (ImGui::Toggle(ImGui::IMGUI_FORMAT_ID("", CHECKBOX_ID, "IS_LOOPING_ANIMATION_MODAL").c_str(), &isLooping))
				{
					m_AnimationTrack.SetIsLooping(isLooping);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}

				ImGui::SameLine();

				ImGui::SetNextItemWidth(100);
				ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "CURRENT_FRAME_ANIMATION_MODAL").c_str(), &m_iCurrentFrame, 1, 0, m_AnimationTrack.GetFrameCount());


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
					ImVec2 legendRectSize = ImVec2((m_AnimationTrack.GetFrameCount() * ANIMATION_FRAME_PIXEL_WIDTH) + LEGEND_PADDING + LEGEND_PADDING, LEGEND_HEIGHT + LEGEND_PADDING);
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
					for (int i = frameMin; i <= m_AnimationTrack.GetFrameCount(); i += frameStep)
					{
						bool baseIndex = ((i % modFrameCount) == 0) || (i == m_AnimationTrack.GetFrameCount() || i == frameMin);
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

						m_iCurrentFrame = std::round(relativePos.x / ANIMATION_FRAME_PIXEL_WIDTH);
						m_iCurrentFrame = std::clamp(m_iCurrentFrame, 0, m_AnimationTrack.GetFrameCount());
					}

					ImGui::SetCursorScreenPos(ImVec2(legendPos.x, legendRectMax.y + LEGEND_PADDING));
					ImVec2 timelinePos = ImGui::GetCursorScreenPos();

					// --- Draw ticks ---
					for (int i = frameMin; i <= m_AnimationTrack.GetFrameCount(); i += frameStep)
					{
						float px = legendPos.x + ((i - frameMin) * ANIMATION_FRAME_PIXEL_WIDTH);

						drawList->AddLine(
							ImVec2(px, timelinePos.y),
							ImVec2(px, timelinePos.y + TRACK_SIZE),
							0xFF606060,
							1.0f
						);
					}

					m_AnimationTrackUIView.Render();
				}

				ImGui::EndChild();
			}

			void AnimationWindow::SetData(gallus::resources::FileResource& a_File)
			{
				m_pFile = &a_File;

				m_AnimationTrack.LoadByPath(a_File.GetPath());
				m_AnimationTrackUIView.Load(m_AnimationTrack);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE