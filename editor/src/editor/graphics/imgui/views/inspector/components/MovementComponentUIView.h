#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

// gameplay includes
#include "gameplay/systems/MovementSystem.h"
#include "gameplay/systems/components/MovementComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the MovementComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class MovementComponentUIView : public ComponentUIView<gameplay::MovementComponent, gameplay::MovementSystem>
			{
			public:
				/// <summary>
				/// Constructs the MovementComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_MovementComponent">The MovementComponent to display and edit.</param>
				/// <param name="a_System">The MovementSystem responsible for managing the MovementComponent.</param>
				MovementComponentUIView(ImGuiWindow& a_Window, gameplay::MovementComponent& a_MovementComponent, gameplay::MovementSystem& a_System) : ComponentUIView(a_Window, a_MovementComponent, a_System)
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
			};
		}
	}
}

REGISTER_COMPONENT_UI(
	gallus::gameplay::MovementComponent,
	gallus::graphics::imgui::MovementComponentUIView,
	gallus::gameplay::MovementSystem
)

#endif // _EDITOR
#endif // IMGUI_DISABLE