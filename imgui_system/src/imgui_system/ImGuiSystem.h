#pragma once

#include "core/System.h"

// external
#include <imgui/imgui.h>
#include <wtypes.h>
#include <memory>

// utils
#include "utils/math.h"

namespace gallus::graphics::dx12
{
	class DX12System;
	class CommandList;
	class Texture;
}
namespace gallus::graphics::imgui
{
	class ImGuiWindowsConfig;

	extern ImVec4 GetAccentColor();

	struct OverlayCommand
	{
		std::function<void(ImDrawList*)> m_fnDraw;
	};

	struct OverlayQueue
	{
		std::vector<OverlayCommand> m_aCommands;

		void Push(std::function<void(ImDrawList*)> fn)
		{
			m_aCommands.push_back({ fn });
		}

		void Flush(ImDrawList* pDrawList, const ImVec2& clipMin, const ImVec2& clipMax)
		{
			pDrawList->PushClipRect(clipMin, clipMax, true);

			for (auto& cmd : m_aCommands)
			{
				cmd.m_fnDraw(pDrawList);
			}

			pDrawList->PopClipRect();
			m_aCommands.clear();
		}
	};

	extern OverlayQueue& GetOverlayQueue();

	//---------------------------------------------------------------------
	// ImGuiSystem
	//---------------------------------------------------------------------
	class ImGuiSystem : public core::System
	{
	public:
		ImGuiSystem();

		/// <summary>
		/// Setup events in the imgui system.
		/// </summary>
		bool SetupEvents();

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
		void OnRenderTargetCreated(graphics::dx12::DX12System& a_System, std::shared_ptr<dx12::CommandList> a_pCommandList);

		/// <summary>
		/// Called when the render target is created.
		/// </summary>
		/// <param name="a_vPos">Position of the window.</param>
		/// <param name="a_vSize">Size of the window.</param>
		void Resize(const IVector2& a_vPos, const IVector2& a_vSize);

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
		/// Retrieves the frame padding.
		/// </summary>
		/// <returns>A vector containing the frame padding settings.</returns>
		const ImVec2& GetFramePadding() const;

		/// <summary>
		/// Retrieves the item spacing.
		/// </summary>
		/// <returns>A vector containing the item spacing settings.</returns>
		const ImVec2& GetItemSpacing() const;

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

		void SetIniPath(const std::string& a_sPath);
	private:
		size_t m_iSrvIndex = 0;

		ImFont* m_pDefaultFont = nullptr;
		ImFont* m_pCapital = nullptr;
		ImFont* m_pCapitalIconFont = nullptr;
		ImFont* m_pBoldFont = nullptr;
		ImFont* m_pIconFont = nullptr;
		ImFont* m_pIconFontM = nullptr;
		ImFont* m_pSmallIconFont = nullptr;

		float m_fFontSize = 0.0f; /// Default font size for ImGui.
		float m_fIconFontSize = 0.0f; /// Default font size for ImGui.

		ImVec2 m_vFramePadding = {}; /// Frame padding for ImGui elements.
		ImVec2 m_vItemSpacing = {}; /// Window padding for ImGui elements.
		ImVec2 m_vHeaderSize;

		ImVec4 m_vAccentColor;
		ImVec4 m_vAccentColorDarker;

		ImGuiWindowsConfig* m_pWindowsConfig = nullptr;

		std::string m_sIniPath;

		friend dx12::DX12System;
	};
}