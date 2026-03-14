#pragma once

#include "imgui_system/views/ImGuiUIView.h"

// external
#include <string>
#include <imgui/imgui.h>

namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	//---------------------------------------------------------------------
	// BaseWindow
	//---------------------------------------------------------------------
	/// <summary>
	/// Base window with functions that can be overridden for specific behaviour.
	/// </summary>
	class BaseWindow : public ImGuiUIView
	{
	public:
		/// <summary>
		/// Constructs a window.
		/// </summary>
		/// <param name="a_System">The ImGui System for rendering the view.</param>
		/// <param name="a_Flags">The flags used for disabling or enabling certain behaviours in the window.</param>
		/// <param name="a_sName">The name of the window (displayed at the top).</param>
		/// <param name="a_sWindowID">The ID of the window.</param>
		/// <param name="a_bFullScreen">Whether the window is full screen or not.</param>
		BaseWindow(ImGuiSystem& a_System, ImGuiWindowFlags a_Flags, const std::string& a_sName, std::string a_sWindowID, bool a_bFullScreen = false);

		/// <summary>
		/// Destructor for BaseWindow.
		/// </summary>
		~BaseWindow();

		/// <summary>
		/// Initializes all behaviours and values for the window.
		/// </summary>
		/// <returns>True if initialization is successful, otherwise false.</returns>
		virtual bool Initialize();

		/// <summary>
		/// Destroys and disables the window.
		/// </summary>
		/// <returns>True if destruction is successful, otherwise false.</returns>
		virtual bool Destroy();

		/// <summary>
		/// Renders the window.
		/// </summary>
		virtual bool WindowBegin();

		/// <summary>
		/// Finalizes rendering of the window.
		/// </summary>
		virtual void WindowEnd();

		/// <summary>
		/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
		/// </summary>
		virtual void Update();

		/// <summary>
		/// Sets the size of the window.
		/// </summary>
		/// <param name="a_vSize">Size of the window.</param>
		void SetSize(ImVec2 a_vSize);

		/// <summary>
		/// Retrieves the full screen state of the window.
		/// </summary>
		/// <returns>True if window is in full screen mode, otherwise false.</returns>
		bool IsFullScreen() const;

		const std::string& GetId()
		{
			return m_sWindowID;
		}

		void Focus()
		{
			m_bFocusMyWindow = true;
		}
	protected:
		bool m_bHideCloseButton = true;
		ImGuiWindowFlags m_Flags = 0; /// The flags associated with certain window behaviours.
		ImVec2 m_vSize = ImVec2(0, 0); /// The current size of the window.
		std::string
			m_sName, /// The name of the window.
			m_sWindowID; /// The ID of the window.
		bool m_bFullScreen = false; /// Whether the window is in full screen mode or not.
		bool m_bEnabled = true; /// Whether the window is enabled or not.
		bool m_bRenderWindowItself = false; /// Whether the window only needs to render the elements or the window around it.
		bool m_bFocusMyWindow = false;
		bool m_bInitialized = false;
		bool m_bBegin = false;
	};
}