#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "AnimationKeyFrameComponentBaseUIView.h"

#include "animation/AnimationKeyFrameEventComponent.h"

#include "animation/AnimationEvents.h"

#include "graphics/imgui/views/DataTypes/StringDropdown.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the animation event component in the ImGui editor.
			/// </summary>
			class AnimationKeyFrameEventComponentUIView : public AnimationKeyFrameComponentUIView<animation::AnimationKeyFrameEventComponent>
			{
			public:
				/// <summary>
				/// Constructs the EventComponentUIView.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EventComponent">The EventComponentUIView to display and edit.</param>
				AnimationKeyFrameEventComponentUIView(ImGuiWindow& a_Window, animation::AnimationKeyFrameEventComponent& a_EventComponent);
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

				StringDropdown<animation::AnimationEvent> m_AnimationEventDropdown; /// Drop down UI element for selecting animation events.
			};
		}
	}
}

REGISTER_KEY_FRAME_COMPONENT_UI(
	gallus::animation::AnimationKeyFrameEventComponent,
	gallus::graphics::imgui::AnimationKeyFrameEventComponentUIView
)

#endif // _EDITOR
#endif // IMGUI_DISABLE