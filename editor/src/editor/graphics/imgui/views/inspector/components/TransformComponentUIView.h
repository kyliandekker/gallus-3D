#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

#include <DirectXMath.h>

// graphics includes
#include "graphics/imgui/views/DataTypes/VectorView.h"

// gameplay includes
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/components/TransformComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the TransformComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class TransformComponentUIView : public ComponentUIView<gameplay::TransformComponent, gameplay::TransformSystem>
			{
			public:
				/// <summary>
				/// Constructs the TransformComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EntityID">The entity ID associated with the transform component.</param>
				/// <param name="a_TransformComponent">The TransformComponent to display and edit.</param>
				/// <param name="a_System">The TransformSystem responsible for managing the TransformComponent.</param>
				TransformComponentUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, gameplay::TransformComponent& a_TransformComponent, gameplay::TransformSystem& a_System) : ComponentUIView(a_Window, a_EntityID, a_TransformComponent, a_System),
					m_PositionView(a_Window),
					m_ScaleView(a_Window),
					m_PivotView(a_Window)
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

				/// UI elements for displaying and editing the position, rotation, and scale as glm::vec3.
				Vector2View<DirectX::XMFLOAT2>
					m_PositionView,
					m_ScaleView,
					m_PivotView;
			};
		}
	}
}

REGISTER_COMPONENT_UI(
	gallus::gameplay::TransformComponent,
	gallus::graphics::imgui::TransformComponentUIView,
	gallus::gameplay::TransformSystem
)

#endif // _EDITOR
#endif // IMGUI_DISABLE