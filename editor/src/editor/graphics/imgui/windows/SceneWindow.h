#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

#include <imgui\ImGuizmo.h>

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
			private:
				void DrawGizmos(const ImVec2& a_vSceneStartPos, const ImVec2& a_vSize);
				void DrawBoundsGizmo(const ImVec2& a_vScenePos);
				void DrawViewportPanel();
				void DrawTransformGizmo();

				float m_fZoom = 1.0f;
				ImVec2 m_vPanOffset = ImVec2(0.0f, 0.0f);
				ImGuizmo::OPERATION m_CurrentOperation = ImGuizmo::TRANSLATE;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE