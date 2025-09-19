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
				/// Constructs a scene window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				SceneWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Cleans up and destroys the scene window.
				/// </summary>
				~SceneWindow();

				/// <summary>
				/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
				/// </summary>
				void Update() override;

				/// <summary>
				/// Renders the scene window.
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

			class FullSceneWindow : public SceneWindow
			{
			public:
				/// <summary>
				/// Constructs a scene window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				FullSceneWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
				/// </summary>
				void Update() override;

				/// <summary>
				/// Renders the scene window.
				/// </summary>
				void Render() override;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE