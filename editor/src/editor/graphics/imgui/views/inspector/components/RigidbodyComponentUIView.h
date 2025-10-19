#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

// gameplay includes
#include "gameplay/systems/RigidbodySystem.h"
#include "gameplay/systems/components/RigidbodyComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the RigidbodyComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class RigidbodyComponentUIView : public ComponentUIView<gameplay::RigidbodyComponent, gameplay::RigidbodySystem>
			{
			public:
				/// <summary>
				/// Constructs the RigidbodyComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_RigidbodyComponent">The RigidbodyComponent to display and edit.</param>
				/// <param name="a_System">The RigidbodySystem responsible for managing the RigidbodyComponent.</param>
				RigidbodyComponentUIView(ImGuiWindow& a_Window, gameplay::RigidbodyComponent& a_RigidbodyComponent, gameplay::RigidbodySystem& a_System) : ComponentUIView(a_Window, a_RigidbodyComponent, a_System)
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
	gallus::gameplay::RigidbodyComponent,
	gallus::graphics::imgui::RigidbodyComponentUIView,
	gallus::gameplay::RigidbodySystem
)

#endif // _EDITOR
#endif // IMGUI_DISABLE