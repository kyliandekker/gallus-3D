#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

#include <DirectXMath.h>

// graphics includes
#include "graphics/imgui/views/DataTypes/VectorView.h"

// gameplay includes
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/components/SpriteComponent.h"

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
			/// UI view for rendering and interacting with the SpriteComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class SpriteComponentUIView : public ComponentUIView<gameplay::SpriteComponent, gameplay::SpriteSystem>
			{
			public:
				/// <summary>
				/// Constructs the SpriteComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_SpriteComponent">The SpriteComponent to display and edit.</param>
				/// <param name="a_System">The SpriteSystem responsible for managing the SpriteComponent.</param>
				SpriteComponentUIView(ImGuiWindow& a_Window, gameplay::SpriteComponent& a_SpriteComponent, gameplay::SpriteSystem& a_System);

				void RenderPreview() override;

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

				char m_sPrefabName[128];
				char m_VertexShaderName[128];
				char m_TextureName[128];
			};
		}
	}
}

REGISTER_COMPONENT_UI(
	gallus::gameplay::SpriteComponent,
	gallus::graphics::imgui::SpriteComponentUIView,
	gallus::gameplay::SpriteSystem
)

#endif // _EDITOR
#endif // IMGUI_DISABLE