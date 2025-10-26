#include "AnimationTrackUIView.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>

#include "animation/SpriteAnimationKeyFrame.h"
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/views/inspector/animation/SpriteAnimationKeyFrameInspectorUIView.h"
#include <graphics/imgui/font_icon.h>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			AnimationTrackUIView::AnimationTrackUIView(ImGuiWindow& a_Window, animation::AnimationTrackBase& a_AnimationTrack) : ImGuiUIView(a_Window), a_AnimationTrack(a_AnimationTrack)
			{
				for (size_t i = 0; i < a_AnimationTrack.GetKeyFramesSize(); i++)
				{
					m_aKeyFrames.push_back(SpriteAnimationKeyFrameUIView(a_Window, *dynamic_cast<animation::SpriteAnimationKeyFrame*>(a_AnimationTrack.GetKeyFrame(i))));
				}
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

				for (SpriteAnimationKeyFrameUIView& keyFrameUIView : m_aKeyFrames)
				{
					int frame = keyFrameUIView.GetKeyFrame().GetFrame();

					float px = startPos.x + (frame * ANIMATION_FRAME_PIXEL_WIDTH);
					std::string keyFrameIcon = font::ICON_KEYFRAME;
					ImVec2 iconSize = ImGui::CalcTextSize(keyFrameIcon.c_str());
					float verticalOffset = (TRACK_SIZE - iconSize.y) / 2.0f;

					ImVec2 contentStartPos = ImVec2(px - (iconSize.x / 2), startPos.y + verticalOffset);
					ImVec2 contentEndPos = ImVec2(contentStartPos.x + iconSize.x, contentStartPos.y + iconSize.y);

					ImGui::SetCursorScreenPos(contentStartPos);

					SpriteAnimationKeyFrameUIView* selectedView = dynamic_cast<SpriteAnimationKeyFrameUIView*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get());
					bool bSelected = (selectedView == &keyFrameUIView);

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
					static SpriteAnimationKeyFrameUIView* s_pDraggedKeyFrame = nullptr;
					static float s_fDragOffset = 0.0f;

					// Start drag
					if (bHovered && bMouseClicked)
					{
						core::EDITOR_ENGINE->GetEditor().SetSelectable(&keyFrameUIView, new SpriteAnimationKeyFrameInspectorUIView(m_Window, keyFrameUIView));
						s_bDragging = true;
						s_pDraggedKeyFrame = &keyFrameUIView;
						s_fDragOffset = io.MousePos.x - px;
					}

					// Update drag
					if (s_bDragging && s_pDraggedKeyFrame == &keyFrameUIView && bMouseDragging)
					{
						float newPx = io.MousePos.x - s_fDragOffset;
						int newFrame = static_cast<int>((newPx - startPos.x) / ANIMATION_FRAME_PIXEL_WIDTH);

						if (newFrame < 0)
						{
							newFrame = 0;
						}

						keyFrameUIView.GetKeyFrame().SetFrame(newFrame);
					}

					// End drag
					if (!bMouseDown && s_bDragging && s_pDraggedKeyFrame == &keyFrameUIView)
					{
						s_bDragging = false;
						s_pDraggedKeyFrame = nullptr;
					}
				}

				drawList->AddLine(
					ImVec2(animationTrackRect.Min.x, animationTrackRect.Max.y),
					animationTrackRect.Max,
					0xFF606060,
					1.0f
				);

				ImGui::SetCursorScreenPos(ImVec2(animationTrackRect.Min.x, animationTrackRect.Max.y));
			}
		}
	}
}