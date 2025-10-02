#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

// base class
#include "ComponentUIView.h"

// external
#include <DirectXMath.h>

// graphics
#include "graphics/imgui/views/DataTypes/VectorView.h"

// gameplay
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
				/// <param name="a_TransformComponent">The TransformComponent to display and edit.</param>
				/// <param name="a_System">The TransformSystem responsible for managing the TransformComponent.</param>
				TransformComponentUIView(ImGuiWindow& a_Window, gameplay::TransformComponent& a_TransformComponent, gameplay::TransformSystem& a_System) : ComponentUIView(a_Window, a_TransformComponent, a_System),
					m_PositionView(a_Window),
					m_RotationView(a_Window),
					m_ScaleView(a_Window),
					m_PivotView(a_Window)
				{}

				void RenderComponentGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom) override;
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
				Vector3View<DirectX::XMFLOAT3>
					m_PositionView,
					m_RotationView,
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