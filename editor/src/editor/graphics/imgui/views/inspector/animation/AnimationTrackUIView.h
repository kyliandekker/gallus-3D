#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

#include "animation/AnimationTrack.h"

#include "editor/graphics/imgui/views/inspector/animation/SpriteAnimationKeyFrameUIView.h"

#include <vector>

constexpr float ANIMATION_FRAME_PIXEL_WIDTH = 25.0f;
constexpr float LEGEND_PADDING = 25.0f;
constexpr float LEGEND_HEIGHT = 70.0f;

namespace gallus
{
	namespace animation
	{
		class AnimationTrackBase;
	}
	namespace graphics
	{
		namespace imgui
		{
			class AnimationTrackUIView : public ImGuiUIView
			{
			public:
				AnimationTrackUIView(ImGuiWindow& a_Window, animation::AnimationTrackBase& a_AnimationTrack);

				void Render() override;

				std::vector<SpriteAnimationKeyFrameUIView> m_aKeyFrames;
				animation::AnimationTrackBase& a_AnimationTrack;
			};
		}
	}
}