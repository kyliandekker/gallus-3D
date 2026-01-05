#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

// external
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
				void Draw2DGrid(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom);
			private:
				void DrawSerializedObjectGizmos(const ImVec2& a_vSceneStartPos, const ImVec2& a_vSize);
				void DrawGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom);
				void DrawViewportPanel();

				void HandleCameraInput(double a_fDeltaTime, const ImVec2& a_vSceneStartPos, const ImVec2& a_vSize);

				float m_fZoom = 1.0f;
				ImVec2 m_vPanOffset = ImVec2(0.0f, 0.0f);
				float m_fCameraPitch = 0.0f;
				float m_fCameraYaw = 0.0f;
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