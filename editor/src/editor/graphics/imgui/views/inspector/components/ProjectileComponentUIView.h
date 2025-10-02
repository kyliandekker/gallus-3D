#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

// base class
#include "ComponentUIView.h"

// gameplay
#include "gameplay/systems/ProjectileSystem.h"
#include "gameplay/systems/components/ProjectileComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the ProjectileComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class ProjectileComponentUIView : public ComponentUIView<gameplay::ProjectileComponent, gameplay::ProjectileSystem>
			{
			public:
				/// <summary>
				/// Constructs the ProjectileComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_ProjectileComponent">The ProjectileComponent to display and edit.</param>
				/// <param name="a_System">The ProjectileSystem responsible for managing the ProjectileComponent.</param>
				ProjectileComponentUIView(ImGuiWindow& a_Window, gameplay::ProjectileComponent& a_ProjectileComponent, gameplay::ProjectileSystem& a_System) : ComponentUIView(a_Window, a_ProjectileComponent, a_System)
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
	gallus::gameplay::ProjectileComponent,
	gallus::graphics::imgui::ProjectileComponentUIView,
	gallus::gameplay::ProjectileSystem
)

#endif // _EDITOR
#endif // IMGUI_DISABLE