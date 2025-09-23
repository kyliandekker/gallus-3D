#ifndef IMGUI_DISABLE

#include "ImGuiWindow.h"

#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx12.h>
#include <imgui/imgui_internal.h>
#include <imgui/implot.h>

// core includes
#include "core/Engine.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/win32/Window.h"
#include "graphics/dx12/DX12System2D.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/imgui/font_arial.h"
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/themes.h"
#include "graphics/imgui/windows/BaseWindow.h"
#include "graphics/imgui/modals/BaseModal.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// ImGuiWindow
			//---------------------------------------------------------------------
			ImGuiWindow::ImGuiWindow() : core::System()
			{}

			std::string m_IniPath;
			//---------------------------------------------------------------------
			bool ImGuiWindow::Initialize()
			{
				IMGUI_CHECKVERSION();
				ImGui::CreateContext();
				ImPlot::CreateContext();

				m_IniPath = fs::path(core::ENGINE->GetSaveDirectory().generic_string() + "/imgui.ini").generic_string();
				ImGuiIO& io = ImGui::GetIO();
				io.IniFilename = m_IniPath.c_str();

				if (!CreateContextWin32() || !CreateContextDX12())
				{
					return false;
				}

				CreateImGui();

				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Initialized ImGui.");

				return System::Initialize();
			}

			//---------------------------------------------------------------------
			bool ImGuiWindow::InitializeWindows()
			{
				for (BaseWindow* window : m_aWindows)
				{
					window->Initialize();
				}
				for (BaseModal* window : m_aModals)
				{
					window->Initialize();
				}
				return true;
			}

			//---------------------------------------------------------------------
			bool ImGuiWindow::Destroy()
			{
				ImGui_ImplDX12_Shutdown();
				ImGui_ImplWin32_Shutdown();
				ImPlot::DestroyContext();
				ImGui::DestroyContext();

				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Destroyed ImGui.");

				return true;
			}

			//---------------------------------------------------------------------
			bool ImGuiWindow::CreateContextWin32()
			{
				if (!ImGui_ImplWin32_Init(core::ENGINE->GetWindow().GetHWnd()))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed creating WIN32 context for ImGui.");
					return false;
				}

				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Created WIN32 context for ImGui.");
				return true;
			}

			//---------------------------------------------------------------------
			bool ImGuiWindow::CreateContextDX12()
			{
				dx12::DX12System2D& dx12window = core::ENGINE->GetDX12();
				m_iSrvIndex = dx12window.GetSRV().Allocate();

				if (!ImGui_ImplDX12_Init(dx12window.GetDevice().Get(), dx12::g_iBufferCount,
					DXGI_FORMAT_R8G8B8A8_UNORM, dx12window.GetSRV().GetHeap().Get(),
					dx12window.GetSRV().GetCPUHandle(m_iSrvIndex),
					dx12window.GetSRV().GetGPUHandle(m_iSrvIndex)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed creating DX12 context for ImGui.");
					return false;
				}

				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Created DX12 context for ImGui.");
				return true;
			}

			//---------------------------------------------------------------------
			void ImGuiWindow::CreateImGui()
			{
				ImGuiIO& io = ImGui::GetIO();
				io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
				io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
				io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;

				(void) io;

				// On Windows 8.1+:
				UINT dpi = GetDpiForWindow(core::ENGINE->GetWindow().GetHWnd()); // returns DPI, e.g., 96, 120, 144
				float dp = dpi / 96.0f; // 96 is the default DPI (100%)

				m_fFontSize *= dp;
				m_fIconFontSize *= dp;

				m_vHeaderSize = ImVec2(m_fFontSize * 3.0f, m_fFontSize * 3.0f);

				// setup Dear ImGui style
				ImGui::StyleColorsDark();

				constexpr ImWchar icons_ranges_b[] = { font::FONT_START, font::FONT_END, 0 };

				ImFontConfig font_config_capital;
				font_config_capital.FontDataOwnedByAtlas = false;
				m_pCapital = io.Fonts->AddFontFromMemoryTTF(&font::arial, sizeof(font::arial), m_fFontSize * 2, &font_config_capital);

				ImFontConfig font_config_icon_capital;
				font_config_icon_capital.FontDataOwnedByAtlas = false;
				m_pCapitalIconFont = io.Fonts->AddFontFromMemoryTTF(&font::ICON, sizeof(font::ICON), m_vHeaderSize.x * 0.5f, &font_config_icon_capital, icons_ranges_b);

				ImFontConfig icons_config_b;
				icons_config_b.FontDataOwnedByAtlas = false;
				m_pIconFont = io.Fonts->AddFontFromMemoryTTF(&font::ICON, sizeof(font::ICON), m_fIconFontSize, &icons_config_b, icons_ranges_b);

				ImFontConfig small_icons_config_b;
				small_icons_config_b.FontDataOwnedByAtlas = false;
				m_pSmallIconFont = io.Fonts->AddFontFromMemoryTTF(&font::ICON, sizeof(font::ICON), m_fFontSize, &small_icons_config_b, icons_ranges_b);

				ImFontConfig font_config_bold;
				font_config_bold.FontDataOwnedByAtlas = false;
				m_pBoldFont = io.Fonts->AddFontFromMemoryTTF(&font::arialBold, sizeof(font::arialBold), m_fFontSize, &font_config_bold);

				ImFontConfig font_config_default;
				font_config_default.FontDataOwnedByAtlas = false;
				m_pDefaultFont = io.Fonts->AddFontFromMemoryTTF(&font::arial, sizeof(font::arial), m_fFontSize, &font_config_default);

				ImFontConfig icons_config_m;
				icons_config_m.MergeMode = true;
				icons_config_m.PixelSnapH = true;
				icons_config_m.FontDataOwnedByAtlas = false;
				m_pIconFontM = io.Fonts->AddFontFromMemoryTTF(&font::ICON, sizeof(font::ICON), m_fFontSize, &icons_config_m, icons_ranges_b);

				io.Fonts->Build();

				ImGui::StyleColorsDark();

				ImGuiStyle& style = ImGui::GetStyle();
				ImVec4* colors = style.Colors;

#define BESTGAMEEVER 1

#ifdef PURPLEBURGLARALARM
				ImVec4 accentColor = ImVec4(0.42f, 0.34f, 0.83f, 1.00f);
#elif BESTGAMEEVER
				ImVec4 accentColor = ImVec4(0.71f, 0.32f, 0.08f, 1.00f);
#elif BLUTARCH
				ImVec4 accentColor = ImVec4(0.35f, 0.5f, 0.6f, 1.00f);
#elif REDMOND
				ImVec4 accentColor = ImVec4(0.63f, 0.36f, 0.32f, 1.00f);
#endif
				colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
				colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
				colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
				colors[ImGuiCol_Header] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_HeaderHovered] = ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.5f);
				//colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_HeaderActive] = accentColor;
				colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
				colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
				colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
				colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_TabActive] = accentColor;
				colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_NavHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
				colors[ImGuiCol_TextColorAccent] = accentColor;
#ifndef _RENDER_TEX
				float transparency = 0.4;
				ImGui::GetStyle().Colors[ImGuiCol_FrameBg].w = transparency;
				ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = transparency;
				ImGui::GetStyle().Colors[ImGuiCol_ChildBg].w = transparency;
#endif // _RENDER_TEX

				style.WindowBorderSize = 1;
				style.WindowRounding = 0;
				style.WindowPadding = ImVec2(0, 0);
				style.ItemSpacing = m_vFramePadding;
				style.ItemInnerSpacing = ImVec2(0, 0);
				style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
				style.IndentSpacing = 25;
				style.ScrollbarSize = 15;
				style.GrabMinSize = 10;
				style.TabRounding = 8;
				style.TabBorderSize = 1;
				style.PopupBorderSize = 1;
				style.FrameBorderSize = 1;
				style.ChildBorderSize = 1;
				style.ChildRounding = 0;
				style.FrameRounding = 0;
				style.PopupRounding = 0;
				style.ScrollbarRounding = 8;
				style.GrabRounding = 8;
				style.LogSliderDeadzone = 4;
				style.FramePadding = ImVec2(0, 0);

				ImPlotStyle& pStyle = ImPlot::GetStyle();
				colors = pStyle.Colors;
				colors[ImPlotCol_Line] = accentColor;
			}

			//---------------------------------------------------------------------
			void ImGuiWindow::OnRenderTargetCreated(std::shared_ptr<dx12::CommandList> a_pCommandList)
			{
				if (!m_bRunning.load())
				{
					return;
				}

				ImGui_ImplDX12_CreateDeviceObjects();
			}

			//---------------------------------------------------------------------
			void ImGuiWindow::Resize(const glm::ivec2& a_vPos, const glm::ivec2& a_vSize)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.DisplaySize.x = static_cast<float>(a_vSize.x);
				io.DisplaySize.y = static_cast<float>(a_vSize.y);
			}

			//---------------------------------------------------------------------
			void ImGuiWindow::OnRenderTargetCleaned()
			{
				if (!m_bRunning.load())
				{
					return;
				}

				ImGui_ImplDX12_InvalidateDeviceObjects();
			}

			//---------------------------------------------------------------------
			void ImGuiWindow::Render(std::shared_ptr<dx12::CommandList> a_pCommandList)
			{
				if (!m_bRunning.load())
				{
					return;
				}

				ImGui_ImplDX12_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				ImGui::PushFont(m_pIconFontM);

				const ImGuiIO& io = ImGui::GetIO();

				if (!m_aWindows.empty())
				{
					m_aWindows[0]->SetSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
					for (BaseWindow* window : m_aWindows)
					{
						window->Update();
					}
				}

				for (BaseModal* modal : m_aModals)
				{
					modal->Update();
				}

				UpdateMouseCursor();

				ImGui::PopFont();

				ImGui::EndFrame();
				ImGui::Render();

				ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), a_pCommandList->GetCommandList().Get());
			}

			//---------------------------------------------------------------------
			void ImGuiWindow::UpdateMouseCursor()
			{
				if (ImGui::IsAnyItemHovered())
				{
					// Set the cursor to a hand pointer
					if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
					{
						ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
					}

					ImGuiIO& io = ImGui::GetIO();
					ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
					if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
					{
						::SetCursor(NULL);
					}
					else
					{
						// Map ImGui cursor types to Win32 system cursors
						LPTSTR win32_cursor = IDC_ARROW; // Default arrow

						switch (imgui_cursor)
						{
							case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
							case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
							case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
							case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
							case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
							case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
							case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
							case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
							default:                            win32_cursor = IDC_ARROW; break;
						}

						// Set the system cursor using Win32 API
						::SetCursor(LoadCursor(NULL, win32_cursor));
					}
				}
			}

			//---------------------------------------------------------------------
			LRESULT ImGuiWindow::WndProcHandler(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
			{
				if (!m_bRunning.load())
				{
					return a_iMsg;
				}

				return ImGui_ImplWin32_WndProcHandler(a_hWnd, a_iMsg, a_wParam, a_lParam);
			}

			//---------------------------------------------------------------------
			ImFont* ImGuiWindow::GetCapitalFont()
			{
				return m_pCapital;
			}

			//---------------------------------------------------------------------
			ImFont* ImGuiWindow::GetBigIconFont()
			{
				return m_pCapitalIconFont;
			}

			//---------------------------------------------------------------------
			ImFont* ImGuiWindow::GetIconFont()
			{
				return m_pIconFont;
			}

			//---------------------------------------------------------------------
			ImFont* ImGuiWindow::GetSmallIconFont()
			{
				return m_pSmallIconFont;
			}

			//---------------------------------------------------------------------
			ImFont* ImGuiWindow::GetBoldFont()
			{
				return m_pBoldFont;
			}

			//---------------------------------------------------------------------
			float ImGuiWindow::GetFontSize() const
			{
				return m_fFontSize;
			}

			//---------------------------------------------------------------------
			const ImVec2& ImGuiWindow::GetFramePadding() const
			{
				return m_vFramePadding;
			}

			//---------------------------------------------------------------------
			const ImVec2& ImGuiWindow::GetWindowPadding() const
			{
				return m_vWindowPadding;
			}

			//---------------------------------------------------------------------
			const ImVec2& ImGuiWindow::GetHeaderSize() const
			{
				return m_vHeaderSize;
			}

			//---------------------------------------------------------------------
			BaseModal* ImGuiWindow::GetModal(int a_iIndex)
			{
				if (a_iIndex >= m_aModals.size())
				{
					return nullptr;
				}
				return m_aModals[a_iIndex];
			}
		}
	}
}

#endif // IMGUI_DISABLE