#ifndef IMGUI_DISABLE

#pragma once

#include "core/System.h"

// external
#include <imgui/imgui.h>
#include <wtypes.h>
#include <memory>
#include <vector>
#include <glm/vec2.hpp>
#include <string>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class DX12System;
			class CommandList;
			class Texture;
		}

		class FileResource;

		namespace imgui
		{
			class ImGuiWindowsConfig;

#define BESTGAMEEVER 1

#ifdef PURPLEBURGLARALARM
			static ImVec4 g_vAccentColor = ImVec4(0.42f, 0.34f, 0.83f, 1.00f);
#elif BESTGAMEEVER
			static ImVec4 g_vAccentColor = ImVec4(0.71f, 0.32f, 0.08f, 1.00f);
#elif BLUTARCH
			static ImVec4 g_vAccentColor = ImVec4(0.35f, 0.5f, 0.6f, 1.00f);
#elif REDMOND
			static ImVec4 g_vAccentColor = ImVec4(0.63f, 0.36f, 0.32f, 1.00f);
#endif
			//---------------------------------------------------------------------
			// ImGuiWindow
			//---------------------------------------------------------------------
			class ImGuiWindow : public core::System
			{
			public:
				ImGuiWindow();

				/// <summary>
				/// Initializes the imgui system.
				/// </summary>
				bool Initialize();

				/// <summary>
				/// Initializes the windows.
				/// </summary>
				bool InitializeWindows();

				/// <summary>
				/// Destroys all imgui resources.
				/// </summary>
				bool Destroy() override;
			private:
				/// <summary>
				/// Handles Windows messages for the editor's window.
				/// </summary>
				/// <param name="a_hWnd">Handle to the window.</param>
				/// <param name="a_iMsg">Message identifier.</param>
				/// <param name="a_wParam">Additional message information (WPARAM).</param>
				/// <param name="a_lParam">Additional message information (LPARAM).</param>
				/// <returns>The result of the message processing.</returns>
				LRESULT CALLBACK WndProcHandler(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);

				/// <summary>
				/// Creates the ImGui context for Win32.
				/// </summary>
				/// <returns>True if the context creation succeeds, otherwise false.</returns>
				bool CreateContextWin32();

				/// <summary>
				/// Creates the ImGui context for DirectX 12.
				/// </summary>
				/// <returns>True if the context creation succeeds, otherwise false.</returns>
				bool CreateContextDX12();

				/// <summary>
				/// Initializes the ImGui UI components.
				/// </summary>
				void CreateImGui();

				/// <summary>
				/// Called when the render target is created.
				/// </summary>
				void OnRenderTargetCreated(std::shared_ptr<dx12::CommandList> a_pCommandList);

				/// <summary>
				/// Called when the render target is created.
				/// </summary>
				/// <param name="a_vPos">Position of the window.</param>
				/// <param name="a_vSize">Size of the window.</param>
				void Resize(const glm::ivec2& a_vPos, const glm::ivec2& a_vSize);

				/// <summary>
				/// Called when the render target is cleaned.
				/// </summary>
				void OnRenderTargetCleaned();

				/// <summary>
				/// Updates the mouse cursor when hovering, clicking, etc.
				/// </summary>
				void UpdateMouseCursor();
			public:
				void Render(std::shared_ptr<dx12::CommandList> a_pCommandList);

				/// <summary>
				/// Retrieves the capital font.
				/// </summary>
				/// <returns>A pointer to the ImFont.</returns>
				ImFont* GetCapitalFont();

				/// <summary>
				/// Retrieves the big icon font.
				/// </summary>
				/// <returns>A pointer to the ImFont.</returns>
				ImFont* GetBigIconFont();

				/// <summary>
				/// Retrieves the icon font.
				/// </summary>
				/// <returns>A pointer to the ImFont.</returns>
				ImFont* GetIconFont();

				/// <summary>
				/// Retrieves the small font.
				/// </summary>
				/// <returns>A pointer to the ImFont.</returns>
				ImFont* GetSmallIconFont();

				/// <summary>
				/// Retrieves the bold font.
				/// </summary>
				/// <returns>A pointer to the ImFont.</returns>
				ImFont* GetBoldFont();

				/// <summary>
				/// Retrieves the font size.
				/// </summary>
				/// <returns>A vector containing the size of the font.</returns>
				float GetFontSize() const;

				/// <summary>
				/// Retrieves the capital font.
				/// </summary>
				/// <returns>A vector containing the frame padding settings.</returns>
				const ImVec2& GetFramePadding() const;

				/// <summary>
				/// Retrieves the capital font.
				/// </summary>
				/// <returns>A vector containing the window padding settings.</returns>
				const ImVec2& GetWindowPadding() const;

				/// <summary>
				/// Retrieves the capital font.
				/// </summary>
				/// <returns>A vector containing the header font size.</returns>
				const ImVec2& GetHeaderSize() const;

				/// <summary>
				/// Retrieves the accent (main) color of the application.
				/// </summary>
				/// <returns>A vector containing the color.</returns>
				const ImVec4& GetAccentColor() const
				{
					return m_vAccentColor;
				}

				/// <summary>
				/// Retrieves the accent (main) color of the application.
				/// </summary>
				/// <returns>A vector containing the color.</returns>
				const ImVec4& GetAccentColorDarker() const
				{
					return m_vAccentColorDarker;
				}

				ImGuiWindowsConfig& GetWindowsConfig()
				{
					return *m_pWindowsConfig;
				}

				template <typename T>
				T& GetWindowsConfig()
				{
					return *reinterpret_cast<T*>(m_pWindowsConfig);
				}

				template <typename T>
				void SetWindowsConfig()
				{
					m_pWindowsConfig = new T(*this);
				}
			private:
				size_t m_iSrvIndex = 0;

				ImFont* m_pDefaultFont = nullptr;
				ImFont* m_pCapital = nullptr;
				ImFont* m_pCapitalIconFont = nullptr;
				ImFont* m_pBoldFont = nullptr;
				ImFont* m_pIconFont = nullptr;
				ImFont* m_pIconFontM = nullptr;
				ImFont* m_pSmallIconFont = nullptr;

				float m_fFontSize = 15.0f; /// Default font size for ImGui.
				float m_fIconFontSize = 15.0f; /// Default font size for ImGui.

				ImVec2 m_vFramePadding = ImVec2(8, 8); /// Frame padding for ImGui elements.
				ImVec2 m_vWindowPadding = ImVec2(8, 8); /// Window padding for ImGui elements.
				ImVec2 m_vItemSpacing = ImVec2(5, 5); /// Window padding for ImGui elements.
				ImVec2 m_vHeaderSize;

				ImVec4 m_vAccentColor;
				ImVec4 m_vAccentColorDarker;

				ImGuiWindowsConfig* m_pWindowsConfig = nullptr;

				friend dx12::DX12System;
			};
		}
	}
}

#endif // IMGUI_DISABLE