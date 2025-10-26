#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "editor/graphics/imgui/EditorSelectable.h"

namespace gallus
{
	namespace animation
	{
		class SpriteAnimationKeyFrame;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			class SpriteAnimationKeyFrameUIView : public EditorSelectable
			{
			public:
				SpriteAnimationKeyFrameUIView(ImGuiWindow& a_Window, animation::SpriteAnimationKeyFrame& a_KeyFrame) : EditorSelectable(),
					m_KeyFrame(a_KeyFrame)
				{

				}

				void RenderEditorSelectable() override
				{}

				animation::SpriteAnimationKeyFrame& GetKeyFrame()
				{
					return m_KeyFrame;
				}

				const animation::SpriteAnimationKeyFrame& GetKeyFrame() const
				{
					return m_KeyFrame;
				}
			private:
				animation::SpriteAnimationKeyFrame& m_KeyFrame;
			};
		}
	}
}

#endif
#endif