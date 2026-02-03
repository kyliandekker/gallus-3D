#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "StatsWindow.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/implot.h>

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/imgui/font_icon.h"

// utils
#include "utils/string_extensions.h"

// gameplay
#include "gameplay/Game.h"

// editor
#include "editor/core/EditorEngine.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
#define MAX_GRAPHICS_FRAMES_STATS 256
#define MAX_GAME_FRAMES_STATS 32

			//---------------------------------------------------------------------
			// StatsWindow
			//---------------------------------------------------------------------
			StatsWindow::StatsWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_STATS) + " Stats", "STATS")
			{
			}

			//---------------------------------------------------------------------
			StatsWindow::~StatsWindow()
			{}

			//---------------------------------------------------------------------
			bool StatsWindow::Initialize()
			{
				PopulateToolbar();

				graphics::dx12::DX12System* dx12System = core::ENGINE->GetDX12();
				if (!dx12System)
				{
					return false;
				}

				dx12System->OnNewFrame() += std::bind(&StatsWindow::OnNewGraphicsFrame, this, std::placeholders::_1);
				gameplay::GAME.OnNewFrame() += std::bind(&StatsWindow::OnNewGameFrame, this, std::placeholders::_1);
				return BaseWindow::Initialize();
			}

			//---------------------------------------------------------------------
			void StatsWindow::PopulateToolbar()
			{
				ImVec2 toolbarSize = ImVec2(0, m_Window.GetHeaderSize().y);
				m_Toolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_ANIMATION"), toolbarSize);

				// Round up button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,

					[this]()
					{
						bool roundUpFPS = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetFPSPrecision();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SETTINGS) + " Round Up Values", BUTTON_ID, "ROUND_UP_FPS_STATS").c_str(), &roundUpFPS, "Rounds up FPS values to zero decimals.", ImVec2(10.5f * m_Window.GetFontSize(), m_Window.GetHeaderSize().y)))
						{
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetFPSPrecision(roundUpFPS);
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
						}
					}
				));
			}

			//---------------------------------------------------------------------
			void StatsWindow::DrawToolbar()
			{
				// Start toolbar.
				m_Toolbar.StartToolbar();

				// Render toolbar.
				m_Toolbar.Render();

				// End toolbar.
				m_Toolbar.EndToolbar();
			}

			//---------------------------------------------------------------------
			void StatsWindow::Update()
			{
				BaseWindow::Update();
			}

			//---------------------------------------------------------------------
			void StatsWindow::Render()
			{
				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				graphics::dx12::DX12System* dx12System = core::ENGINE->GetDX12();
				if (!dx12System)
				{
					return;
				}

				DrawToolbar();

				ImGui::SetCursorPos({
					ImGui::GetCursorPos().x + m_Window.GetFramePadding().x,
					ImGui::GetCursorPos().y + m_Window.GetFramePadding().y,
				});

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetWindowPadding());
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_Window.GetWindowPadding());
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, m_Window.GetWindowPadding());

				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "STATS").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					true,
					ImGuiWindowFlags_AlwaysVerticalScrollbar
				))
				{
					bool roundUpFPS = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetFPSPrecision();
					if (gameplay::GAME.IsStarted())
					{
						ImGui::Indent();
						bool tableActive = ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "STATS").c_str(), dx12System->GetImGuiWindow().GetFramePadding());

						if (tableActive)
						{
							float graphicsFPS = dx12System->GetFPS().GetFPS();
							float gameFPS = gameplay::GAME.GetFPS().GetFPS();

							if (roundUpFPS)
							{
								graphicsFPS = std::round(graphicsFPS);
								gameFPS = std::round(gameFPS);
							}
							ImGui::KeyValue([]
							{
								graphics::dx12::DX12System* dx12System = core::ENGINE->GetDX12();
								if (!dx12System)
								{
									return;
								}

								ImGui::AlignTextToFramePadding();
								ImGui::DisplayHeader(dx12System->GetImGuiWindow().GetBoldFont(), "Graphics FPS");
							}, [&graphicsFPS, &roundUpFPS]
							{
								if (roundUpFPS)
								{
									ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.0f", graphicsFPS);
								}
								else
								{
									ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.6f", graphicsFPS);
								}
								return false;
							});
							ImGui::KeyValue([]
							{
								graphics::dx12::DX12System* dx12System = core::ENGINE->GetDX12();
								if (!dx12System)
								{
									return;
								}

								ImGui::AlignTextToFramePadding();
								ImGui::DisplayHeader(dx12System->GetImGuiWindow().GetBoldFont(), "Game FPS");
							}, [&gameFPS, &roundUpFPS]
							{
								if (roundUpFPS)
								{
									ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.0f", gameFPS);
								}
								else
								{
									ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.6f", gameFPS);
								}
								return false;
							});

							ImGui::EndInspectorKeyVal(ImVec2());
						}

						ImGui::Unindent();
						if (ImPlot::BeginPlot(ImGui::IMGUI_FORMAT_ID("", PLOT_ID, "GRAPHICS_FPS_STATS").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 300), ImPlotFlags_CanvasOnly | ImPlotFlags_NoChild | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
						{
							ImPlot::SetupAxis(ImAxis_X1, "");
							ImPlot::SetupAxis(ImAxis_Y1, "");
							ImPlot::SetupAxisLimits(ImAxis_Y1, 0.f, dx12System->GetFPS().GetTargetFPS() + 15, ImPlotCond_Always);
							ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_GRAPHICS_FRAMES_STATS);

							ImPlot::PlotLine("Graphics FPS",
								m_aGraphicsFPSValues.data(),
								static_cast<int>(m_aGraphicsFPSValues.size()));

							ImPlot::EndPlot();
						}

						if (ImPlot::BeginPlot(ImGui::IMGUI_FORMAT_ID("", PLOT_ID, "GAME_FPS_STATS").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 300), ImPlotFlags_CanvasOnly | ImPlotFlags_NoChild | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
						{
							ImPlot::SetupAxis(ImAxis_X1, "");
							ImPlot::SetupAxis(ImAxis_Y1, "");
							ImPlot::SetupAxisLimits(ImAxis_Y1, 0.f, gameplay::GAME.GetFPS().GetTargetFPS() + 15, ImPlotCond_Always);
							ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_GAME_FRAMES_STATS);

							ImPlot::PlotLine("Game FPS",
								m_aGameFPSValues.data(),
								static_cast<int>(m_aGameFPSValues.size()));

							ImPlot::EndPlot();
						}
					}
				}
				ImGui::EndChild();

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			//---------------------------------------------------------------------
			void StatsWindow::OnNewGraphicsFrame(float a_fFPS)
			{
				m_aGraphicsFPSValues.push_back(a_fFPS);
				if (m_aGraphicsFPSValues.size() > MAX_GRAPHICS_FRAMES_STATS)
				{
					m_aGraphicsFPSValues.erase(m_aGraphicsFPSValues.begin());
				}
			}

			//---------------------------------------------------------------------
			void StatsWindow::OnNewGameFrame(float a_fFPS)
			{
				m_aGameFPSValues.push_back(a_fFPS);
				if (m_aGameFPSValues.size() > MAX_GAME_FRAMES_STATS)
				{
					m_aGameFPSValues.erase(m_aGameFPSValues.begin());
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE