#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

// external
#include <functional>
#include <imgui/ImGuizmo.h>
#include <imgui/imgui_internal.h>

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

			class ToolbarItem
			{
			public:
				virtual void Render() = 0;
			};

			//---------------------------------------------------------------------
			// ToolbarButton
			//---------------------------------------------------------------------
			class ToolbarButton : public ToolbarItem
			{
			public:
				ToolbarButton(std::function<void()> a_ButtonFunc, std::function<bool()> a_DisabledCondition = []()
					{
						return false;
					});

				void Render() override;
			private:
				std::function<void()> m_ButtonFunc;
				std::function<bool()> m_DisabledCondition;
			};

			//---------------------------------------------------------------------
			// ToolbarBreak
			//---------------------------------------------------------------------
			class ToolbarBreak : public ToolbarItem
			{
			public:
				ToolbarBreak(const ImVec2& a_vBreakSize) : m_vBreakSize(a_vBreakSize)
				{}

				void Render() override;
			private:
				ImVec2 m_vBreakSize = {};
			};

			//---------------------------------------------------------------------
			// Toolbar
			//---------------------------------------------------------------------
			class Toolbar
			{
			public:
				Toolbar() = default;
				Toolbar(const ImVec2& a_vSize) : m_vToolbarSize(a_vSize)
				{}

				void StartToolbar();
				void EndToolbar();

				void Render();

				const ImVec2& GetToolbarSize() const
				{
					return m_vToolbarSize;
				}

				std::vector<ToolbarButton> m_aButtons;
			private:
				ImVec2 m_vToolbarStartScreenPos = {};
				ImVec2 m_vToolbarStartPos = {};
				ImVec2 m_vToolbarSize = {};
			};

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
				virtual void PopulateToolbar();
				void DrawToolbar();

				void HandleViewportControls();

				graphics::dx12::Texture* GetRenderTexture() const;
				ImRect GetRenderTextureRect(gallus::graphics::dx12::Texture* a_pTexture) const;

				void DrawSceneView(gallus::graphics::dx12::Texture* a_pTexture, const ImRect& a_SceneViewRect) const;
				void DrawSceneBackground(const ImRect& a_SceneViewRect) const;
				void SetupSceneGizmos( const ImRect& a_SceneViewRect) const;
				void DrawSceneGrid(const ImRect& a_SceneViewRect) const;
				void DrawSceneTexture(gallus::graphics::dx12::Texture* a_pTexture, const ImRect& a_SceneViewRect) const;
				void DrawSceneGizmos( const ImRect& a_SceneViewRect) const;
				void DrawSceneViewBorder(const ImRect& a_SceneViewRect) const;

				void HandleSceneViewControls(double a_fDeltaTime, const ImRect& a_vSceneRect);

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
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE