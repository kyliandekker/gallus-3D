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
			AnimationTrackUIView::AnimationTrackUIView(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
			{
			}

			void AnimationTrackUIView::Load(animation::AnimationTrack& a_AnimationTrack)
			{
				m_pAnimationTrack = &a_AnimationTrack;
			}

			constexpr float TRACK_SIZE = 50;
			void AnimationTrackUIView::Render()
			{
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImVec2 startPos = ImGui::GetCursorScreenPos();

				ImRect animationTrackRect = ImRect(
					startPos,
					startPos + ImVec2(ImGui::GetContentRegionAvail().x, TRACK_SIZE)
				);

				ImGuiIO& io = ImGui::GetIO();

				int i = 0;
				for (animation::AnimationKeyFrame& keyFrame : m_pAnimationTrack->GetKeyFrames())
				{
					float px = startPos.x + (keyFrame.GetFrame() * ANIMATION_FRAME_PIXEL_WIDTH);
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
						m_iSelectedKeyFrame = i;

						s_bDragging = true;
						s_pDraggedKeyFrame = &keyFrame;
					}

					// Update drag
					if (s_bDragging && s_pDraggedKeyFrame == &keyFrame && bMouseDragging)
					{
						ImVec2 mousePos = ImGui::GetMousePos();
						ImVec2 s_vRelativePos = mousePos - startPos;
						keyFrame = std::round(s_vRelativePos.x / ANIMATION_FRAME_PIXEL_WIDTH);
					}

					// End drag
					if (!bMouseDown && s_bDragging && s_pDraggedKeyFrame == &keyFrame)
					{
						s_bDragging = false;
						s_pDraggedKeyFrame = nullptr;
					}

					i++;
				}

				ImGui::SetCursorScreenPos(ImVec2(startPos.x, animationTrackRect.Max.y));
			}
		}
	}
}