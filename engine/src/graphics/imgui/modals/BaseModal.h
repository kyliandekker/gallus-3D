#ifdef _EDITOR

#pragma once

// base class
#include "graphics/imgui/views/ImGuiUIView.h"

// external
#include <string>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			//---------------------------------------------------------------------
			// BaseModal
			//---------------------------------------------------------------------
			/// <summary>
			/// Base modal with functions that can be overridden for specific behaviour.
			/// </summary>
			class BaseModal : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs a window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_sName">The name of the window (displayed at the top).</param>
				/// <param name="a_sWindowID">The ID of the window.</param>
				/// <param name="a_bFullScreen">Whether the window is full screen or not.</param>
				BaseModal(ImGuiWindow& a_Window, const std::string& a_sName, const std::string& a_sWindowID, bool a_bFullScreen = false);

				/// <summary>
				/// Initializes all behaviours and values for the window.
				/// </summary>
				/// <returns>True if initialization is successful, otherwise false.</returns>
				virtual bool Initialize();

				/// <summary>
				/// Renders the modal.
				/// </summary>
				virtual bool ModalBegin();

				/// <summary>
				/// Finalizes rendering of the modal.
				/// </summary>
				virtual void ModalEnd();

				/// <summary>
				/// Update loop for the modal. This is where all ImGui interaction should be like buttons, etc.
				/// </summary>
				void Update();

				/// <summary>
				/// Shows the modal.
				/// </summary>
				virtual void Show();

				/// <summary>
				/// Hides the modal.
				/// </summary>
				void Hide();
			protected:
				std::string
					m_sName, /// The name of the window.
					m_sWindowID; /// The ID of the window.

				bool m_bShow = false; // Whether the modal should be shown.
			};
		}
	}
}

#endif // _EDITOR