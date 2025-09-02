#pragma once

#include "core/System.h"

#include <glm/vec2.hpp>
#include <wtypes.h>
#include <string>
#include <queue>

// core includes
#include "core/Event.h"

// gameplay includes
#include "graphics/win32/WindowSettings.h"

#if defined(CreateWindow)
#undef CreateWindow
#endif

namespace gallus
{
	namespace graphics
	{
		namespace win32
		{
			//---------------------------------------------------------------------
			// WindowMsg
			//---------------------------------------------------------------------
			struct WindowsMsg
			{
				HWND hwnd;
				UINT msg;
				WPARAM wParam;
				LPARAM lParam;
			};

			//---------------------------------------------------------------------
			// Window
			//---------------------------------------------------------------------
			/// <summary>
			/// Represents a win32 window in the application, with functionality for event polling and rendering 
			/// </summary>
			class Window : public core::ThreadedSystem
			{
			public:
				/// <summary>
				/// Constructs a window.
				/// </summary>
				Window();

				/// <summary>
				/// Initializes the system, setting up necessary resources.
				/// </summary>
				/// <param name="a_bWait">Determines whether the application waits until the system has been fully initialized.</param>
				/// <param name="a_hInstance">Handle to the instance of the program.</param>
				/// <returns>True if the initialization was successful, otherwise false.</returns>
				bool Initialize(bool a_bWait, HINSTANCE a_hInstance);

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
				/// Loop method for the thread.
				/// </summary>
				void Loop() override;

				/// <summary>
				/// Signals the thread to stop.
				/// </summary>
				bool Destroy() override;

				/// <summary>
				/// Makes the window visible on the screen.
				/// </summary>
				void Show();

				/// <summary>
				/// Hides the window from the screen.
				/// </summary>
				void Hide();

				/// <summary>
				/// Retrieves the full screen state of the window.
				/// </summary>
				/// <returns>True if the screen is full screen, otherwise false.</returns>
				bool IsFullScreen() const;

				/// <summary>
				/// Toggles full screen mode for the window.
				/// </summary>
				void ToggleFullscreen();

				/// <summary>
				/// Sets the window title.
				/// </summary>
				/// <param name="a_sTitle">The new title for the window.</param>
				void SetTitle(const std::string& a_sTitle);

				/// <summary>
				/// Adds to the window title.
				/// </summary>
				/// <param name="a_sTitle">The title for the window.</param>
				void AddTitle(const std::string& a_sTitle);

				/// <summary>
				/// Retrieves the real (physical) size of the window.
				/// </summary>
				/// <returns>A 2D vector representing the width and height of the window.</returns>
				glm::ivec2 GetRealSize() const;

				/// <summary>
				/// Retrieves the position of the window on the screen.
				/// </summary>
				/// <returns>A 2D vector representing the x and y position of the window.</returns>
				glm::ivec2 GetPosition() const;

				/// <summary>
				/// Sets the size of the window.
				/// </summary>
				/// <param name="a_vSize">The size of the window</param>
				void SetSize(const glm::ivec2& a_vSize);

				/// <summary>
				/// Sets the position of the window.
				/// </summary>
				/// <param name="a_vPosition">The position the window will be rendered at.</param>
				void SetPosition(const glm::ivec2& a_vPosition);

				/// <summary>
				/// Retrieves the handle to the window.
				/// </summary>
				/// <returns>A reference to the window handle (HWND).</returns>
				HWND& GetHWnd();

				/// <summary>
				/// Retrieves the handle to the instance.
				/// </summary>
				/// <returns>A reference to the instance handle (hInstance).</returns>
				HINSTANCE& GethInstance();

				/// <summary>
				/// Retrieves the windows class for the window.
				/// </summary>
				/// <returns></returns>
				WNDCLASSEX& GetWc();

				/// <summary>
				/// Retrieves the on quit event.
				/// </summary>
				/// <returns>Reference to the on quit event.</returns>
				const SimpleEvent<>& OnQuit() const;

				/// <summary>
				/// Retrieves the event queue.
				/// </summary>
				/// <returns>Reference to the event queue.</returns>
				std::queue<WindowsMsg>& GetEventQueue();

				/// <summary>
				/// Saves window settings.
				/// </summary>
				void SaveSettings();

				void SetResizingAllowed(bool a_bResizingAllowed);

				std::mutex g_EventMutex;
			protected:
				bool Sleep() const override
				{
					return true;
				}
			private:
				/// <summary>
				/// Sets the window title.
				/// </summary>
				/// <param name="a_sTitle">The title for the window.</param>
				void SetTitleInternal(const std::string& a_sTitle);

				LPTSTR m_Cursor = IDC_ARROW;

				/// <summary>
				/// Generic size method that changes the size and position of the window.
				/// </summary>
				/// <param name="a_vPosition">The position the window will be rendered at.</param>
				/// <param name="a_vSize">The size of the window</param>
				void ChangeSize(const glm::ivec2& a_vPosition, const glm::ivec2& a_vSize);

				/// <summary>
				/// Called once on the thread to perform initialization steps.
				/// </summary>
				/// <returns>True if the initialization was successful, otherwise false.</returns>
				bool InitThreadWorker() override;

				/// <summary>
				/// Destroys the system, releasing resources and performing necessary cleanup.
				/// </summary>
				/// <returns>True if the destruction was successful, otherwise false.</returns>
				void Finalize() override;

				/// <summary>
				/// Creates a win32 window.
				/// </summary>
				/// <param name="a_hInstance">The handle instance of the application.</param>
				/// <returns></returns>
				bool CreateWindow(HINSTANCE a_hInstance);

				WindowSettings m_WindowSettings;

				SimpleEvent<> m_OnQuit;
				std::queue<WindowsMsg> m_EventQueue;

				HWND m_hWnd = NULL;
				WNDCLASSEX m_Wc = WNDCLASSEX();
				HINSTANCE m_hInstance = NULL;
				RECT m_WindowRect;

				std::string m_sTitle;

				bool m_bFullScreen = false;
			};
		}
	}
}