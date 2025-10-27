#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

#include "animation/AnimationTrack.h"
#include "animation/AnimationKeyFrame.h"

//#include "editor/graphics/imgui/views/inspector/animation/AnimationKeyFrameUIView.h"

#include <vector>

constexpr float ANIMATION_FRAME_PIXEL_WIDTH = 25.0f;
constexpr float LEGEND_PADDING = 25.0f;
constexpr float LEGEND_HEIGHT = 70.0f;

namespace gallus
{
	namespace animation
	{
		class AnimationTrack;
	}
	namespace graphics
	{
		namespace imgui
		{
			class AnimationTrackUIView : public ImGuiUIView
			{
			public:
				AnimationTrackUIView(ImGuiWindow& a_Window);
				void Load(animation::AnimationTrack& a_AnimationTrack);

				void Render() override;

				animation::AnimationTrack* m_pAnimationTrack = nullptr;

				int GetSelectedKeyFrame() const
				{
					return m_iSelectedKeyFrame;
				}

				void SetSelectedKeyFrame(int a_iSelectedKeyFrame)
				{
					m_iSelectedKeyFrame = a_iSelectedKeyFrame;
				}

				int m_iSelectedKeyFrame = -1;
			};
		}
	}
}