#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "AnimationEditorModal.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_toggle.h>

// core includes
#include "editor/core/EditorEngine.h"

// utils includes
#include "utils/string_extensions.h"

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"

// graphics includes
#include "resources/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/graphics/imgui/views/inspector/animation/AnimationKeyFrameComponentBaseUIView.h"

#include "animation/AnimationKeyFrameSpriteComponent.h"
#include "animation/AnimationKeyFrameEventComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			AnimationEditorModal::AnimationEditorModal(ImGuiWindow& a_Window) : BaseModal(a_Window, std::string(font::ICON_ANIMATION) + " Animation Editor", "AnimationEditor"),
				m_AnimationTrackUIView(a_Window)
			{ }

			void AnimationEditorModal::Render()
			{
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

				ImGui::SetCursorPos(
					ImVec2(
						ImGui::GetCursorPos().x + m_Window.GetFramePadding().x,
						ImGui::GetCursorPos().y + m_Window.GetFramePadding().y
					)
				);

				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "LEGEND_ANIMATION_MODAL").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX,
					ImGuiWindowFlags_AlwaysHorizontalScrollbar
				))
				{
					ImVec2 initialPos = ImGui::GetCursorScreenPos();
					ImVec2 legendRectPos = initialPos ;
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

				ImGui::SameLine();

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2());
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "KEY_FRAME_COMPONENTS_ANIMATION_MODAL").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders
				))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_Window.GetFramePadding());
					if (m_AnimationTrackUIView.m_iSelectedKeyFrame > -1 && m_AnimationTrackUIView.m_iSelectedKeyFrame < m_AnimationTrack.GetKeyFrames().size())
					{
						animation::AnimationKeyFrame& keyFrame = m_AnimationTrack.GetKeyFrames()[m_AnimationTrackUIView.m_iSelectedKeyFrame];

						m_AnimationTrackUIView.GetKeyFrameUIView().Render();

						float width = ImGui::GetContentRegionAvail().x;

						if (!keyFrame.HasComponent<animation::AnimationKeyFrameSpriteComponent>())
						{
							if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_IMAGE + std::string(" Add Sprite Component"), BUTTON_ID, "ADD_SPRITE_COMPONENT_ANIMATION_MODAL").c_str(), ImVec2(width, 0)))
							{
								keyFrame.AddComponent<animation::AnimationKeyFrameSpriteComponent>();
								m_AnimationTrackUIView.GetKeyFrameUIView().SetKeyFrame(keyFrame);
							}
						}
						if (!keyFrame.HasComponent<animation::AnimationKeyFrameEventComponent>())
						{
							if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_CIRCLE_ERROR + std::string(" Add Event Component"), BUTTON_ID, "ADD_EVENT_COMPONENT_ANIMATION_MODAL").c_str(), ImVec2(width, 0)))
							{
								keyFrame.AddComponent<animation::AnimationKeyFrameEventComponent>();
								m_AnimationTrackUIView.GetKeyFrameUIView().SetKeyFrame(keyFrame);
							}
						}
					}
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}


			void AnimationEditorModal::Show()
			{
				BaseModal::Show();
			}

			void AnimationEditorModal::SetData(gallus::resources::FileResource& a_File)
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