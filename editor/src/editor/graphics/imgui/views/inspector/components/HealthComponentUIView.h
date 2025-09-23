#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

// gameplay includes
#include "gameplay/systems/HealthSystem.h"
#include "gameplay/systems/components/HealthComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the HealthComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class HealthComponentUIView : public ComponentUIView<gameplay::HealthComponent, gameplay::HealthSystem>
			{
			public:
				/// <summary>
				/// Constructs the HealthComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EntityID">The entity ID associated with the transform component.</param>
				/// <param name="a_HealthComponent">The HealthComponent to display and edit.</param>
				/// <param name="a_System">The HealthSystem responsible for managing the HealthComponent.</param>
				HealthComponentUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, gameplay::HealthComponent& a_HealthComponent, gameplay::HealthSystem& a_System) : ComponentUIView(a_Window, a_EntityID, a_HealthComponent, a_System)
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
	gallus::gameplay::HealthComponent,
	gallus::graphics::imgui::HealthComponentUIView,
	gallus::gameplay::HealthSystem
)

#endif // _EDITOR
#endif // IMGUI_DISABLE