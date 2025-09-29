#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

// graphics includes
#include "graphics/imgui/views/DataTypes/VectorView.h"

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
				/// <param name="a_ColliderComponent">The ColliderComponent to display and edit.</param>
				/// <param name="a_System">The ColliderSystem responsible for managing the ColliderComponent.</param>
				ColliderComponentUIView(ImGuiWindow& a_Window, gameplay::ColliderComponent& a_ColliderComponent, gameplay::CollisionSystem& a_System) : ComponentUIView(a_Window, a_ColliderComponent, a_System),
					m_OffsetView(a_Window),
					m_SizeView(a_Window)
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
				Vector2View<DirectX::XMFLOAT2>
					m_OffsetView,
					m_SizeView;

				int8_t m_iSelectedHandle = -1;
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