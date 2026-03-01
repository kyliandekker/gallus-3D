#pragma once

#include "imgui_system/windows/BaseWindow.h"

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
			class ImGuiSystem;

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
				/// <param name="a_System">The ImGui system for rendering the view.</param>
				SceneWindow(ImGuiSystem& a_System);

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

				// Keybinds.
				void RegisterKeybinds();

				// Viewport.
				void HandleViewportControls();

				// Render texture.
				graphics::dx12::Texture* GetRenderTexture() const;
				virtual ImRect GetRenderTextureRect(graphics::dx12::Texture* a_pTexture) const;

				// Scene view
				void DrawSceneView(graphics::dx12::Texture* a_pTexture, const ImRect& a_SceneViewRect) const;
				void DrawSceneBackground(const ImRect& a_SceneViewRect) const;
				void SetupSceneGizmos( const ImRect& a_SceneViewRect) const;
				void DrawSceneGrid(const ImRect& a_SceneViewRect) const;
				void DrawSceneTexture(graphics::dx12::Texture* a_pTexture, const ImRect& a_SceneViewRect) const;
				void DrawSceneGizmos( const ImRect& a_SceneViewRect) const;
				void DrawSceneViewBorder(const ImRect& a_SceneViewRect) const;
				
				// Scene view controls.
				void HandleSceneViewControls(double a_fDeltaTime, const ImRect& a_vSceneRect);

				void DrawViewportPanel();

				float m_fZoom = 1.0f;
				ImVec2 m_vPanOffset = ImVec2(0.0f, 0.0f);

				Toolbar m_TopToolbar;

				bool m_bViewportChanged = false;
				float m_fMoveSpeed = 20.0f;
			};

			class FullSceneWindow : public SceneWindow
			{
			public:
				/// <summary>
				/// Constructs a scene window.
				/// </summary>
				/// <param name="a_System">The ImGui system for rendering the view.</param>
				FullSceneWindow(ImGuiSystem& a_System);

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
				
				ImRect GetRenderTextureRect(graphics::dx12::Texture* a_pTexture) const override;

				void DrawFPS(const ImRect& a_SceneViewRect);
			};
		}
	}
}