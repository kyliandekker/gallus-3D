#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

// external
#include <vector>

// editor
#include "editor/graphics/imgui/views/Toolbar.h"

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
			// StatsWindow
			//---------------------------------------------------------------------
			/// <summary>
			/// A window that displays the stats of the game.
			/// </summary>
			class StatsWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs a stats window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				StatsWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Initializes all values and behaviours associated with the hierarchy window.
				/// </summary>
				/// <returns>True if initialization is successful, otherwise false.</returns>
				bool Initialize() override;

				/// <summary>
				/// Cleans up and destroys the stats window.
				/// </summary>
				~StatsWindow();

				/// <summary>
				/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
				/// </summary>
				void Update() override;

				/// <summary>
				/// Renders the stats window.
				/// </summary>
				void Render() override;

				void OnNewGraphicsFrame(float a_fFPS);
				void OnNewGameFrame(float a_fFPS);
			protected:
				// Toolbar.
				void PopulateToolbar();
				void DrawToolbar();

				std::vector<float> m_aGraphicsFPSValues;
				std::vector<float> m_aGameFPSValues;

				Toolbar m_Toolbar;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE