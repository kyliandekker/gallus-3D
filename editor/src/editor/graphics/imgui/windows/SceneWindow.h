#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

// external
#include <imgui/ImGuizmo.h>
#include <imgui/imgui_internal.h>

#include "editor/graphics/imgui/views/Toolbar.h"

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

				bool Initialize() override;

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
			protected:
				// Toolbar.
				void PopulateBaseToolbar();
				virtual void PopulateToolbar();
				void DrawToolbar();

				// Viewport.
				void HandleViewportControls();

				// Render texture.
				graphics::dx12::Texture* GetRenderTexture() const;
				virtual ImRect GetRenderTextureRect(gallus::graphics::dx12::Texture* a_pTexture) const;

				// Scene view
				void DrawSceneView(gallus::graphics::dx12::Texture* a_pTexture, const ImRect& a_SceneViewRect) const;
				void DrawSceneBackground(const ImRect& a_SceneViewRect) const;
				void SetupSceneGizmos( const ImRect& a_SceneViewRect) const;
				void DrawSceneGrid(const ImRect& a_SceneViewRect) const;
				void DrawSceneTexture(gallus::graphics::dx12::Texture* a_pTexture, const ImRect& a_SceneViewRect) const;
				void DrawSceneGizmos( const ImRect& a_SceneViewRect) const;
				void DrawSceneViewBorder(const ImRect& a_SceneViewRect) const;
				
				// Scene view controls.
				void HandleSceneViewControls(double a_fDeltaTime, const ImRect& a_vSceneRect);

				// TODO: Unused for now.
				void DrawViewportPanel();

				float m_fZoom = 1.0f;
				ImVec2 m_vPanOffset = ImVec2(0.0f, 0.0f);

				Toolbar m_Toolbar;
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
			private:
				void PopulateToolbar() override;
				
				ImRect GetRenderTextureRect(gallus::graphics::dx12::Texture* a_pTexture) const override;

				void DrawFPS(const ImRect& a_SceneViewRect);
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE