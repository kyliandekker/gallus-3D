#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "StatsWindow.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/implot.h>

// graphics
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

			bool StatsWindow::Initialize()
			{
				core::EDITOR_ENGINE->GetDX12().OnNewFrame() += std::bind(&StatsWindow::OnNewGraphicsFrame, this, std::placeholders::_1);
				gameplay::GAME.OnNewFrame() += std::bind(&StatsWindow::OnNewGameFrame, this, std::placeholders::_1);
				return true;
			}

			//---------------------------------------------------------------------
			StatsWindow::~StatsWindow()
			{}

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

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				ImVec2 startToolbarPos = ImGui::GetCursorPos();
				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				bool roundUpFPS = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetFPSPrecision();
				if (ImGui::CheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SETTINGS) + " Round Up Values", BUTTON_ID, "ROUND_UP_FPS_STATS").c_str(), &roundUpFPS, ImVec2(150, m_Window.GetHeaderSize().y)))
				{
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetFPSPrecision(roundUpFPS);
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				}

				ImGui::EndToolbar(ImVec2(0, 0));
				ImGui::SetCursorPos(ImVec2(startToolbarPos.x + m_Window.GetFramePadding().y, startToolbarPos.y + toolbarSize.y + m_Window.GetFramePadding().y));

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

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
					if (gameplay::GAME.IsStarted())
					{
						ImGui::Indent();
						bool tableActive = ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "STATS").c_str(), core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFramePadding());

						if (tableActive)
						{
							float graphicsFPS = core::EDITOR_ENGINE->GetDX12().GetFPS().GetFPS();
							float gameFPS = gameplay::GAME.GetFPS().GetFPS();

							if (roundUpFPS)
							{
								graphicsFPS = std::round(graphicsFPS);
								gameFPS = std::round(gameFPS);
							}
							ImGui::KeyValue([]
							{
								ImGui::AlignTextToFramePadding();
								ImGui::DisplayHeader(core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetBoldFont(), "Graphics FPS");
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
								ImGui::AlignTextToFramePadding();
								ImGui::DisplayHeader(core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetBoldFont(), "Game FPS");
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
						if (ImPlot::BeginPlot(ImGui::IMGUI_FORMAT_ID("", PLOT_ID, "GRAPHICS_FPS_STATS").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 300), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
						{
							ImPlot::SetupAxis(ImAxis_X1, "");
							ImPlot::SetupAxis(ImAxis_Y1, "");
							ImPlot::SetupAxisLimits(ImAxis_Y1, 0.f, core::EDITOR_ENGINE->GetDX12().GetFPS().GetTargetFPS() + 15, ImPlotCond_Always);
							ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_GRAPHICS_FRAMES_STATS);

							ImPlot::PlotLine("Graphics FPS",
								m_aGraphicsFPSValues.data(),
								static_cast<int>(m_aGraphicsFPSValues.size()));

							ImPlot::EndPlot();
						}

						if (ImPlot::BeginPlot(ImGui::IMGUI_FORMAT_ID("", PLOT_ID, "GAME_FPS_STATS").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 300), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
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