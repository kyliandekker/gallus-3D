#include "graphics/win32/Window.h"

// external
#include <iostream>
#ifndef IMGUI_DISABLE
#include <imgui/imgui.h>
#endif

// logger
#include "logger/Logger.h"

namespace gallus
{
	namespace graphics
	{
		namespace win32
		{
			//---------------------------------------------------------------------
			LRESULT CALLBACK WndProc(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
			{
				Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(a_hWnd, GWLP_USERDATA));
				if (window)
				{
					return window->WndProcHandler(a_hWnd, a_iMsg, a_wParam, a_lParam);
				}
				return DefWindowProc(a_hWnd, a_iMsg, a_wParam, a_lParam);
			}

			//---------------------------------------------------------------------
			// Window
			//---------------------------------------------------------------------
			Window::Window() : m_WindowSettings("windowsettings.settings")
			{}

			//---------------------------------------------------------------------
			bool Window::Initialize(bool a_bWait, HINSTANCE a_hInstance)
			{
				m_hInstance = a_hInstance;

				LOG(LOGSEVERITY_INFO, LOG_CATEGORY_WINDOW, "Initializing window.");
				return ThreadedSystem::Initialize(a_bWait);
			}

			//---------------------------------------------------------------------
			LRESULT Window::WndProcHandler(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
			{
				m_eOnWinProc();
				switch (a_iMsg)
				{
					case WM_DESTROY:
					{
						PostQuitMessage(0);
						m_eOnQuit();
						return 0;
					}
					case WM_EXITSIZEMOVE:
					{
						SaveSettings();
						break;
					}
				}
				// NOTE: We push ALL events because ImGui for instance needs events.
				// Lock the queue and push the event
				{
					std::lock_guard<std::mutex> lock(g_EventMutex);
					m_EventQueue.push({ a_hWnd, a_iMsg, a_wParam, a_lParam });
				}

				return DefWindowProc(a_hWnd, a_iMsg, a_wParam, a_lParam);
			}

			//---------------------------------------------------------------------
			void Window::Loop()
			{
				MSG msg = {};
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			//---------------------------------------------------------------------
			bool Window::Destroy()
			{
				LOG(LOGSEVERITY_INFO, LOG_CATEGORY_WINDOW, "Destroying window.");
				return ThreadedSystem::Destroy();
			}

			//---------------------------------------------------------------------
			void Window::Show()
			{
				::ShowWindow(m_hWnd, 1);
				::UpdateWindow(m_hWnd);
			}

			//---------------------------------------------------------------------
			void Window::Hide()
			{
				::ShowWindow(m_hWnd, 0);
			}

			//---------------------------------------------------------------------
			bool Window::IsFullScreen() const
			{
				return m_bFullScreen;
			}

			//---------------------------------------------------------------------
			void Window::ToggleFullscreen()
			{
				m_bFullScreen = !m_bFullScreen;

				if (m_bFullScreen)
				{
					// Save the current window position and size
					::GetWindowRect(m_hWnd, &m_WindowRect);

					// Adjust window style to borderless fullscreen
					UINT windowStyle = WS_POPUP;
					::SetWindowLongW(m_hWnd, GWL_STYLE, windowStyle);

					// Get monitor information for fullscreen dimensions
					HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
					MONITORINFO monitorInfo = {};
					monitorInfo.cbSize = sizeof(MONITORINFO);

					if (::GetMonitorInfo(hMonitor, &monitorInfo))
					{
						// Resize and reposition the window to cover the monitor
						::SetWindowPos(m_hWnd, HWND_TOP,
							monitorInfo.rcMonitor.left,
							monitorInfo.rcMonitor.top,
							monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
							monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
							SWP_FRAMECHANGED | SWP_NOACTIVATE);

						::ShowWindow(m_hWnd, SW_MAXIMIZE);
					}
				}
				else
				{
					// Restore the window style
					::SetWindowLongW(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

					// Restore the saved window position and size
					::SetWindowPos(m_hWnd, HWND_NOTOPMOST,
						m_WindowRect.left, m_WindowRect.top,
						m_WindowRect.right - m_WindowRect.left,
						m_WindowRect.bottom - m_WindowRect.top,
						SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_SHOWWINDOW);

					// Restore the window to normal state
					::ShowWindow(m_hWnd, SW_NORMAL);
					::UpdateWindow(m_hWnd);           // Force immediate redraw
				}
			}

			//---------------------------------------------------------------------
			void Window::SetTitle(const std::string& a_sTitle)
			{
				m_sTitle = a_sTitle;
				SetTitleInternal("");
			}

			//---------------------------------------------------------------------
			void Window::AddTitle(const std::string& a_sTitle)
			{
				SetTitleInternal(a_sTitle);
			}

			//---------------------------------------------------------------------
			void Window::SetTitleInternal(const std::string& a_sTitle)
			{
				std::string title = m_sTitle + a_sTitle.c_str();
				::SetWindowTextA(m_hWnd, title.c_str());
			}

			//---------------------------------------------------------------------
			glm::ivec2 Window::GetRealSize() const
			{
				RECT rect;
				GetClientRect(m_hWnd, &rect);

				const glm::ivec2 size = glm::ivec2(rect.right - rect.left, rect.bottom - rect.top);
				return size;
			}

			//---------------------------------------------------------------------
			glm::ivec2 Window::GetPosition() const
			{
				RECT rect;
				GetWindowRect(m_hWnd, &rect); // This gives screen coordinates

				return glm::ivec2(rect.left, rect.top);
			}

			//---------------------------------------------------------------------
			void Window::SetSize(const glm::ivec2& a_vSize)
			{
				RECT rc = { 0, 0, a_vSize.x, a_vSize.y };
				AdjustWindowRectEx(&rc, GetWindowLong(m_hWnd, GWL_STYLE), FALSE, GetWindowLong(m_hWnd, GWL_EXSTYLE));

				const glm::ivec2 totalSize(rc.right - rc.left, rc.bottom - rc.top);

				ChangeSize(GetPosition(), totalSize);
			}

			//---------------------------------------------------------------------
			void Window::SetPosition(const glm::ivec2& a_vPosition)
			{
				RECT rect;
				// Get the current window position and size
				GetWindowRect(m_hWnd, &rect);

				ChangeSize(a_vPosition, glm::ivec2(rect.right - rect.left, rect.bottom - rect.top));
			}

			//---------------------------------------------------------------------
			void Window::ChangeSize(const glm::ivec2& a_vPosition, const glm::ivec2& a_vSize)
			{
				SetWindowPos(
					m_hWnd,                // Handle to the window
					HWND_TOP,            // Placement order (HWND_TOP keeps it in its current Z-order)
					a_vPosition.x,
					a_vPosition.y,
					a_vSize.x,
					a_vSize.y,
					SWP_NOZORDER // Flags to retain position and Z-order
				);
			}

			//---------------------------------------------------------------------
			HWND& Window::GetHWnd()
			{
				return m_hWnd;
			}

			//---------------------------------------------------------------------
			HINSTANCE& Window::GethInstance()
			{
				return m_hInstance;
			}

			//---------------------------------------------------------------------
			WNDCLASSEX& Window::GetWc()
			{
				return m_Wc;
			}

			//---------------------------------------------------------------------
			const SimpleEvent<>& Window::OnQuit() const
			{
				return m_eOnQuit;
			}

			//---------------------------------------------------------------------
			std::queue<WindowsMsg>& Window::GetEventQueue()
			{
				return m_EventQueue;
			}

			//---------------------------------------------------------------------
			void Window::SaveSettings()
			{
				m_WindowSettings.SetPosition(GetPosition());
				m_WindowSettings.SetSize(GetRealSize());

				m_WindowSettings.Save();
			}

			void Window::SetResizingAllowed(bool a_bResizingAllowed)
			{
				LONG style = GetWindowLong(m_hWnd, GWL_STYLE);

				if (a_bResizingAllowed)
				{
					style |= WS_THICKFRAME;     // Allow resizing border
					style |= WS_MAXIMIZEBOX;    // Allow maximize button
				}
				else
				{
					style &= ~WS_THICKFRAME;    // Disable resizing
					style &= ~WS_MAXIMIZEBOX;   // Disable maximize button
				}

				SetWindowLong(m_hWnd, GWL_STYLE, style);

				// Apply the changes
				SetWindowPos(
					m_hWnd, nullptr, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
				);
			}

			//---------------------------------------------------------------------
			bool Window::InitThreadWorker()
			{
				CreateWindow(m_hInstance);

				m_WindowSettings.Load();
				SetSize(m_WindowSettings.GetSize());
				SetPosition(m_WindowSettings.GetPosition());

				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_WINDOW, "Initialized window.");

				return true;
			}

			//---------------------------------------------------------------------
			void Window::Finalize()
			{
				::DestroyWindow(m_hWnd);
				::UnregisterClassA(m_Wc.lpszClassName, m_Wc.hInstance);

				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_WINDOW, "Destroyed window.");
			}

			//---------------------------------------------------------------------
			bool Window::CreateWindow(HINSTANCE a_hInstance)
			{
				m_Wc = {};

				m_Wc.cbSize = sizeof(WNDCLASSEX);
				m_Wc.style = CS_HREDRAW | CS_VREDRAW;
				m_Wc.cbClsExtra = 0;
				m_Wc.cbWndExtra = 0;

				m_Wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
				m_Wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

				m_Wc.lpszClassName = "Window";
				m_Wc.lpszMenuName = NULL;

				m_Wc.hInstance = a_hInstance;

				m_Wc.lpfnWndProc = WndProc;

				if (!RegisterClassEx(&m_Wc))
				{
					LOG(LOGSEVERITY_ASSERT, LOG_CATEGORY_WINDOW, "Failed window registration.");
					return false;
				}

				SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

				m_hWnd = CreateWindowEx(
					WS_EX_CLIENTEDGE,
					"Window",
					"Game Window",
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, CW_USEDEFAULT, 230, 400,
					NULL, NULL, a_hInstance, NULL);

				if (!m_hWnd)
				{
					LOG(LOGSEVERITY_ASSERT, LOG_CATEGORY_WINDOW, "Failed window creation.");
					return false;
				}

				SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

				Show();
				return true;
			}
		}
	}
}