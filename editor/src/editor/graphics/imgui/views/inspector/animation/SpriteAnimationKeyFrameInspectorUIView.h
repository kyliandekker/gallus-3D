#pragma once

#include "editor/graphics/imgui/views/inspector/InspectorView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class SpriteAnimationKeyFrameUIView;

			class SpriteAnimationKeyFrameInspectorUIView : public InspectorView
			{
			public:
				SpriteAnimationKeyFrameInspectorUIView(ImGuiWindow& a_Window, SpriteAnimationKeyFrameUIView& a_SpriteAnimationKeyFrameUIView) : InspectorView(a_Window),
					m_SpriteAnimationKeyFrameUIView(a_SpriteAnimationKeyFrameUIView)
				{
					m_bShowDelete = true;
					m_bShowPreview = true;
				}

				std::string GetName() const override
				{
					return "Sprite Animation Key Frame";
				}

				std::string GetIcon() const override;

				void Render() override;

				void RenderPreview() override;
			private:
				SpriteAnimationKeyFrameUIView& m_SpriteAnimationKeyFrameUIView;
				char m_TextureName[128];
			};
		}
	}
}