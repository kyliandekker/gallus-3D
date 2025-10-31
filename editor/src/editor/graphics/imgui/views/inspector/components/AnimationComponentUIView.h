#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

// gameplay includes
#include "gameplay/systems/AnimationSystem.h"
#include "gameplay/systems/components/AnimationComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the AnimationComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class AnimationComponentUIView : public ComponentUIView<gameplay::AnimationComponent, gameplay::AnimationSystem>
			{
			public:
				/// <summary>
				/// Constructs the AnimationComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_AnimationComponent">The AnimationComponent to display and edit.</param>
				/// <param name="a_System">The AnimationSystem responsible for managing the AnimationComponent.</param>
				AnimationComponentUIView(ImGuiWindow& a_Window, gameplay::AnimationComponent& a_AnimationComponent, gameplay::AnimationSystem& a_System) : ComponentUIView(a_Window, a_AnimationComponent, a_System)
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

				char m_sStartingAnimation[128];
			};
		}
	}
}

REGISTER_COMPONENT_UI(
	gallus::gameplay::AnimationComponent,
	gallus::graphics::imgui::AnimationComponentUIView,
	gallus::gameplay::AnimationSystem
)

#endif // _EDITOR
#endif // IMGUI_DISABLE