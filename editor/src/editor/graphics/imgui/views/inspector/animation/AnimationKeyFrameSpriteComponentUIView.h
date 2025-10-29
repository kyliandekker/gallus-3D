#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "AnimationKeyFrameComponentBaseUIView.h"

#include "animation/AnimationKeyFrameSpriteComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the ColliderComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class AnimationKeyFrameSpriteComponentUIView : public AnimationKeyFrameComponentUIView<animation::AnimationKeyFrameSpriteComponent>
			{
			public:
				/// <summary>
				/// Constructs the SpriteComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_SpriteComponent">The SpriteComponentUIView to display and edit.</param>
				AnimationKeyFrameSpriteComponentUIView(ImGuiWindow& a_Window, animation::AnimationKeyFrameSpriteComponent& a_SpriteComponent) : AnimationKeyFrameComponentUIView(a_Window, a_SpriteComponent)
				{}
			private:
				/// <summary>
				/// Render the inner part of the UI, including position, rotation, scale, and color.
				/// </summary>
				void RenderInner() override;

				/// <summary>
				/// Retrieves the name of the component UI.
				/// </summary>
				/// <returns>The name of the UI component.</returns>
				std::string GetName() const override;

				char m_TextureName[128];
			};
		}
	}
}

REGISTER_KEY_FRAME_COMPONENT_UI(
	gallus::animation::AnimationKeyFrameSpriteComponent,
	gallus::graphics::imgui::AnimationKeyFrameSpriteComponentUIView
)

#endif // _EDITOR
#endif // IMGUI_DISABLE