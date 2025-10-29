#include "AnimationTrackUIView.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>

#include "editor/core/EditorEngine.h"
#include <graphics/imgui/font_icon.h>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			AnimationTrackUIView::AnimationTrackUIView(ImGuiWindow& a_Window) : ImGuiUIView(a_Window),
				m_AnimationKeyFrameUIView(a_Window)
			{
			}

			void AnimationTrackUIView::Load(animation::AnimationTrack& a_AnimationTrack)
			{
				m_pAnimationTrack = &a_AnimationTrack;
			}

			void AnimationTrackUIView::Render()
			{
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImVec2 startPos = ImGui::GetCursorScreenPos();

				ImRect animationTrackRect = ImRect(
					startPos,
					startPos + ImVec2(ImGui::GetContentRegionAvail().x, TRACK_SIZE)
				);

				ImGuiIO& io = ImGui::GetIO();

				ANIMATION_FRAME_PIXEL_WIDTH = ANIMATION_FRAME_PIXEL_WIDTH_DEFAULT;
				float calcLegendWidth = (m_pAnimationTrack->GetFrameCount() * ANIMATION_FRAME_PIXEL_WIDTH_DEFAULT) - LEGEND_PADDING;
				if (calcLegendWidth < ImGui::GetContentRegionAvail().x)
				{
					ANIMATION_FRAME_PIXEL_WIDTH = (ImGui::GetContentRegionAvail().x - LEGEND_PADDING) / m_pAnimationTrack->GetFrameCount();
				}
	
				int i = 0;
				for (animation::AnimationKeyFrame* keyFrame : m_pAnimationTrack->GetKeyFrames())
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
						SetSelectedKeyFrame(i);

						s_bDragging = true;
						s_pDraggedKeyFrame = keyFrame;
					}

					// Update drag
					if (s_bDragging && s_pDraggedKeyFrame == keyFrame && bMouseDragging)
					{
						ImVec2 mousePos = ImGui::GetMousePos();
						ImVec2 s_vRelativePos = mousePos - startPos;
						int frame = std::round(s_vRelativePos.x / ANIMATION_FRAME_PIXEL_WIDTH);
						keyFrame->SetFrame(std::clamp(frame, 0, m_pAnimationTrack->GetFrameCount()));
					}

					// End drag
					if (!bMouseDown && s_bDragging && s_pDraggedKeyFrame == keyFrame)
					{
						s_bDragging = false;
						s_pDraggedKeyFrame = nullptr;
					}

					i++;
				}

				ImRect lines = ImRect(
					startPos - ImVec2(LEGEND_PADDING, 0),
					startPos - ImVec2(LEGEND_PADDING, 0) + ImVec2(ImGui::GetContentRegionAvail().x, TRACK_SIZE)
				);

				drawList->AddLine(
					lines.Min,
					ImVec2(lines.Max.x, lines.Min.y),
					0xFFFFFFFF,
					1.0f
				);

				drawList->AddLine(
					ImVec2(lines.Min.x, lines.Max.y),
					ImVec2(lines.Max.x, lines.Max.y),
					0xFFFFFFFF,
					1.0f
				);

				ImGui::SetCursorScreenPos(ImVec2(startPos.x, animationTrackRect.Max.y));
			}

			void AnimationTrackUIView::SetSelectedKeyFrame(int a_iSelectedKeyFrame)
			{
				if (a_iSelectedKeyFrame < 0)
				{
					return;
				}

				m_iSelectedKeyFrame = a_iSelectedKeyFrame;
				m_AnimationKeyFrameUIView.SetKeyFrame(*m_pAnimationTrack->GetKeyFrames()[a_iSelectedKeyFrame]);
			}
		}
	}
}