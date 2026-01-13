#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

// external
#include <vector>

namespace gallus
{
	namespace resources
	{
		class EngineResource;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			//---------------------------------------------------------------------
			// ResourcesWindow
			//---------------------------------------------------------------------
			/// <summary>
			/// A window that displays the resources currently loaded in the atlas.
			/// </summary>
			class ResourcesWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs a resources window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ResourcesWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Cleans up and destroys the resources window.
				/// </summary>
				~ResourcesWindow();

				/// <summary>
				/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
				/// </summary>
				void Update() override;

				/// <summary>
				/// Renders the resources window.
				/// </summary>
				void Render() override;
			private:
				void RenderResource(resources::EngineResource* a_pResource);

				bool
					m_bTexturesFoldedOut = false,
					m_bMeshesFoldedOut = false,
					m_bMaterialsFoldedOut = false,
					m_bPixelShadersFoldedOut = false,
					m_bVertexShadersFoldedOut = false;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE