#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

				//---------------------------------------------------------------------
				// SceneWindow
				//---------------------------------------------------------------------
				/// <summary>
				/// A window that displays the scene.
				/// </summary>
			class SceneWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs a console window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				SceneWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Cleans up and destroys the console window.
				/// </summary>
				~SceneWindow();

				/// <summary>
				/// Renders the console window.
				/// </summary>
				void Render() override;

				void DrawGizmos(const ImVec2& a_SceneStartPos, const ImVec2& a_Size);
				void DrawTransformGizmo();
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE