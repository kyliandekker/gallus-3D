#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

// gameplay includes
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/components/ColliderComponent.h"

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
			class ColliderComponentUIView : public ComponentUIView<gameplay::ColliderComponent, gameplay::CollisionSystem>
			{
			public:
				/// <summary>
				/// Constructs the ColliderComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EntityID">The entity ID associated with the transform component.</param>
				/// <param name="a_ColliderComponent">The ColliderComponent to display and edit.</param>
				/// <param name="a_System">The ColliderSystem responsible for managing the ColliderComponent.</param>
				ColliderComponentUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, gameplay::ColliderComponent& a_ColliderComponent, gameplay::CollisionSystem& a_System) : ComponentUIView(a_Window, a_EntityID, a_ColliderComponent, a_System)
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
	gallus::gameplay::ColliderComponent,
	gallus::graphics::imgui::ColliderComponentUIView,
	gallus::gameplay::CollisionSystem
)

#endif // _EDITOR
#endif // IMGUI_DISABLE