#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ComponentUIView.h"

#include <DirectXMath.h>
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/components/SpriteComponent.h"

namespace gallus
{
	namespace graphics
	{
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
				/// <param name="a_EntityID">The entity ID associated with the transform component.</param>
				/// <param name="a_SpriteComponent">The SpriteComponent to display and edit.</param>
				/// <param name="a_System">The SpriteSystem responsible for managing the SpriteComponent.</param>
				SpriteComponentUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, gameplay::SpriteComponent& a_SpriteComponent, gameplay::SpriteSystem& a_System) : ComponentUIView(a_Window, a_EntityID, a_SpriteComponent, a_System)
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

#endif // _EDITOR
#endif // IMGUI_DISABLE