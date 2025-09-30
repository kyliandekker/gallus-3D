#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

#include <DirectXMath.h>

// graphics includes
#include "graphics/imgui/views/DataTypes/VectorView.h"

// gameplay includes
#include "gameplay/systems/MeshSystem.h"
#include "gameplay/systems/components/MeshComponent.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
		}
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the MeshComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class MeshComponentUIView : public ComponentUIView<gameplay::MeshComponent, gameplay::MeshSystem>
			{
			public:
				/// <summary>
				/// Constructs the MeshComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_MeshComponent">The MeshComponent to display and edit.</param>
				/// <param name="a_System">The MeshSystem responsible for managing the MeshComponent.</param>
				MeshComponentUIView(ImGuiWindow& a_Window, gameplay::MeshComponent& a_MeshComponent, gameplay::MeshSystem& a_System);
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
				char m_sVertexShaderName[128];
				char m_sTextureName[128];
				char m_sMeshName[128];
			};
		}
	}
}

REGISTER_COMPONENT_UI(
	gallus::gameplay::MeshComponent,
	gallus::graphics::imgui::MeshComponentUIView,
	gallus::gameplay::MeshSystem
)

#endif // _EDITOR
#endif // IMGUI_DISABLE