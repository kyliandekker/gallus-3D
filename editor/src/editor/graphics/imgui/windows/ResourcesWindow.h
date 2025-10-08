#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

// base class
#include "graphics/imgui/windows/BaseWindow.h"

// external
#include <vector>
#include <mutex>

namespace gallus
{
	namespace core
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
			/// A window that displays and manages the logger messages of the project.
			/// </summary>
			class ResourcesWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs a console window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ResourcesWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Cleans up and destroys the console window.
				/// </summary>
				~ResourcesWindow();

				/// <summary>
				/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
				/// </summary>
				void Update() override;

				/// <summary>
				/// Renders the console window.
				/// </summary>
				void Render() override;
			private:
				void RenderResource(core::EngineResource* a_pResource);

				bool
					m_bTexturesFoldedOut = false,
					m_bMeshesFoldedOut = false,
					m_bPixelShadersFoldedOut = false,
					m_bVertexShadersFoldedOut = false,
					m_bShaderBindsFoldedOut = false;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE