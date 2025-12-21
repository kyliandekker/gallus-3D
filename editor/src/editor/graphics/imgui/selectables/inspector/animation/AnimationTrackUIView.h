#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

// external
#include <vector>

// animation
#include "animation/Animation.h"
#include "animation/AnimationKeyFrame.h"

constexpr float ANIMATION_FRAME_PIXEL_WIDTH_DEFAULT = 25.0f;
inline float ANIMATION_FRAME_PIXEL_WIDTH = 25.0f;
constexpr float LEGEND_PADDING = 25.0f;
constexpr float LEGEND_HEIGHT = 70.0f;
constexpr float TRACK_SIZE = 50;

namespace gallus
{
	namespace animation
	{
		class Animation;
	}
	namespace graphics
	{
		namespace imgui
		{
			class AnimationUIView : public ImGuiUIView
			{
			public:
				AnimationUIView(ImGuiWindow& a_Window);
				void Load(animation::Animation& a_Animation);

				void Render() override;

				int GetSelectedKeyFrame() const
				{
					return m_iSelectedKeyFrame;
				}

				void SetSelectedKeyFrame(int a_iSelectedKeyFrame);

				int m_iSelectedKeyFrame = -1;
			private:
				animation::Animation* m_pAnimation = nullptr;
			};
		}
	}
}