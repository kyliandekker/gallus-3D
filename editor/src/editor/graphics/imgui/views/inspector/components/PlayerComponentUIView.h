#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

// gameplay includes
#include "gameplay/systems/PlayerSystem.h"
#include "gameplay/systems/components/PlayerComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the PlayerComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class PlayerComponentUIView : public ComponentUIView<gameplay::PlayerComponent, gameplay::PlayerSystem>
			{
			public:
				/// <summary>
				/// Constructs the PlayerComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_PlayerComponent">The PlayerComponent to display and edit.</param>
				/// <param name="a_System">The PlayerSystem responsible for managing the PlayerComponent.</param>
				PlayerComponentUIView(ImGuiWindow& a_Window, gameplay::PlayerComponent& a_PlayerComponent, gameplay::PlayerSystem& a_System) : ComponentUIView(a_Window, a_PlayerComponent, a_System)
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

				char m_sPrefabName[128];
			};
		}
	}
}

REGISTER_COMPONENT_UI(
	gallus::gameplay::PlayerComponent,
	gallus::graphics::imgui::PlayerComponentUIView,
	gallus::gameplay::PlayerSystem
)

#endif // _EDITOR
#endif // IMGUI_DISABLE