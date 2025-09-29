#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ConsoleWindow.h"

#include <imgui/imgui_helpers.h>

// utils includes
#include "utils/string_extensions.h"

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// editor includes
#include "editor/core/EditorEngine.h"

// game includes
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// ConsoleWindow
			//---------------------------------------------------------------------
			ConsoleWindow::ConsoleWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_CONSOLE) + " Console", "Console"), m_SearchBar(a_Window)
			{
				m_SearchBar.Initialize("");

				// We want every log message. Not just the ones after ImGui has been initialized.
				logger::LOGGER.OnMessageLogged() += std::bind(&ConsoleWindow::LoggerCallback, this, std::placeholders::_1);
			}

			//---------------------------------------------------------------------
			ConsoleWindow::~ConsoleWindow()
			{}

			//---------------------------------------------------------------------
			void ConsoleWindow::Update()
			{
				if (gameplay::GAME.IsStarted() && !gameplay::GAME.IsPaused())
				{
					return;
				}
				BaseWindow::Update();
			}

			std::string logo_arr[8] =
			{
				font::ICON_CIRCLE_ASSERT,
				font::ICON_CIRCLE_ERROR,
				font::ICON_CIRCLE_WARNING,
				font::ICON_CIRCLE_INFO,
				font::ICON_CIRCLE_TEST,
				font::ICON_CIRCLE_SUCCESS,
				font::ICON_CIRCLE_INFO_SUCCESS,
				font::ICON_CIRCLE_AWESOME
			};

			ImVec4 colors_arr[8] =
			{
				ImGui::ConvertColorsRgba(255, 0, 0, 255),
				ImGui::ConvertColorsRgba(255, 0, 0, 255),
				ImGui::ConvertColorsRgba(255, 216, 0, 255),
				ImGui::ConvertColorsRgba(0, 148, 255, 255),
				ImGui::ConvertColorsRgba(0, 148, 255, 255),
				ImGui::ConvertColorsRgba(76, 255, 0, 255),
				ImGui::ConvertColorsRgba(127, 146, 255, 255),
				ImGui::ConvertColorsRgba(255, 110, 220, 255),
			};

			//---------------------------------------------------------------------
			void ConsoleWindow::Render()
			{
				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				editor::EditorSettings& editorSettings = core::EDITOR_ENGINE->GetEditor().GetEditorSettings();

				// Filter messages if need be.
				if (m_bNeedsRefresh)
				{
					std::vector<bool> filters =
					{
						editorSettings.GetShowAssert(),
						editorSettings.GetShowError(),
						editorSettings.GetShowWarning(),
						editorSettings.GetShowInfo(),
						editorSettings.GetShowTest(),
						editorSettings.GetShowSuccess(),
						editorSettings.GetShowInfoSuccess(),
						editorSettings.GetShowAwesome(),
					};

					// Mutex to ensure new messages cannot be added.
					std::lock_guard<std::mutex> lock(MESSAGE_MUTEX);

					m_bNeedsRefresh = false;
					m_aFilteredMessages.clear();

					std::string searchString = string_extensions::StringToLower(m_SearchBar.GetString());
					for (size_t i = 0; i < m_aMessages.size(); i++)
					{
						auto& message = m_aMessages[i];
						if (filters[message.GetSeverity()])
						{
							if (searchString.empty() || ((string_extensions::StringToLower(message.GetRawMessage()).find(searchString) != std::string::npos) || (string_extensions::StringToLower(message.GetCategory()).find(searchString) != std::string::npos)))
							{
								m_aFilteredMessages.push_back(i);
							}
						}
					}
				}

				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				float topPosY = ImGui::GetCursorPosY();

				ImGui::PushFont(m_Window.GetIconFont());
				if (ImGui::TextButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_CLEAR), BUTTON_ID, "CLEAR_CONSOLE").c_str(), m_Window.GetHeaderSize()))
				{
					std::lock_guard<std::mutex> lock(MESSAGE_MUTEX);
					m_aFilteredMessages.clear();
					m_aMessages.clear();
				}
				ImGui::PopFont();

				ImGui::SameLine();
				bool scrollToBottom = editorSettings.GetScrollToBottom();
				if (ImGui::CheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SCROLL_TO_BOTTOM) + " Scroll to bottom", BUTTON_ID, "SCROLL_TO_BOTTOM_CONSOLE").c_str(), &scrollToBottom, ImVec2(190, toolbarSize.y)))
				{
					editorSettings.SetScrollToBottom(scrollToBottom);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}

				ImGui::SameLine();
				bool showInfo = editorSettings.GetShowInfo();
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(logo_arr[LOGSEVERITY_INFO], BUTTON_ID, "SHOW_INFO_CONSOLE").c_str(), &showInfo, m_Window.GetHeaderSize(), m_Window.GetIconFont(), colors_arr[LOGSEVERITY_INFO]))
				{
					editorSettings.SetShowInfo(showInfo);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}

				ImGui::SameLine();
				bool showTest = editorSettings.GetShowTest();
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(logo_arr[LOGSEVERITY_TEST], BUTTON_ID, "SHOW_TEST_CONSOLE").c_str(), &showTest, m_Window.GetHeaderSize(), m_Window.GetIconFont(), colors_arr[LOGSEVERITY_TEST]))
				{
					editorSettings.SetShowTest(showTest);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}

				ImGui::SameLine();
				bool showWarning = editorSettings.GetShowWarning();
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(logo_arr[LOGSEVERITY_WARNING], BUTTON_ID, "SHOW_WARNING_CONSOLE").c_str(), &showWarning, m_Window.GetHeaderSize(), m_Window.GetIconFont(), colors_arr[LOGSEVERITY_WARNING]))
				{
					editorSettings.SetShowWarning(showWarning);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}

				ImGui::SameLine();
				bool showError = editorSettings.GetShowError();
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(logo_arr[LOGSEVERITY_ERROR], BUTTON_ID, "SHOW_ERROR_CONSOLE").c_str(), &showError, m_Window.GetHeaderSize(), m_Window.GetIconFont(), colors_arr[LOGSEVERITY_ERROR]))
				{
					editorSettings.SetShowError(showError);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}

				ImGui::SameLine();
				bool showAssert = editorSettings.GetShowAssert();
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(logo_arr[LOGSEVERITY_ASSERT], BUTTON_ID, "SHOW_ASSERT_CONSOLE").c_str(), &showAssert, m_Window.GetHeaderSize(), m_Window.GetIconFont(), colors_arr[LOGSEVERITY_ASSERT]))
				{
					editorSettings.SetShowAssert(showAssert);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}

				ImGui::SameLine();
				bool showSuccess = editorSettings.GetShowSuccess();
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(logo_arr[LOGSEVERITY_SUCCESS], BUTTON_ID, "SHOW_SUCCESS_CONSOLE").c_str(), &showSuccess, m_Window.GetHeaderSize(), m_Window.GetIconFont(), colors_arr[LOGSEVERITY_SUCCESS]))
				{
					editorSettings.SetShowSuccess(showSuccess);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}

				ImGui::SameLine();
				bool showInfoSuccess = editorSettings.GetShowInfoSuccess();
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(logo_arr[LOGSEVERITY_INFO_SUCCESS], BUTTON_ID, "SHOW_INFO_SUCCESS_CONSOLE").c_str(), &showInfoSuccess, m_Window.GetHeaderSize(), m_Window.GetIconFont(), colors_arr[LOGSEVERITY_INFO_SUCCESS]))
				{
					editorSettings.SetShowInfoSuccess(showInfoSuccess);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}

				ImGui::SameLine();
				bool showAwesome = editorSettings.GetShowAwesome();
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(logo_arr[LOGSEVERITY_AWESOME], BUTTON_ID, "SHOW_AWESOME_CONSOLE").c_str(), &showAwesome, m_Window.GetHeaderSize(), m_Window.GetIconFont(), colors_arr[LOGSEVERITY_AWESOME]))
				{
					editorSettings.SetShowAwesome(showAwesome);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}

				ImVec2 endPos = ImGui::GetCursorPos();

				float searchbarWidth = 300;
				float inputPadding = m_Window.GetFramePadding().x / 2;
				ImVec2 searchBarPos = ImVec2(
					ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (searchbarWidth + m_Window.GetWindowPadding().x),
					(topPosY + (toolbarSize.y / 2)) - (((inputPadding * 2) + m_Window.GetFontSize()) / 2)
				);
				ImGui::SetCursorPos(searchBarPos);
				if (m_SearchBar.Render(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SEARCHBAR_CONSOLE").c_str(), ImVec2(searchbarWidth, toolbarSize.y), inputPadding))
				{
					m_bNeedsRefresh = true;
				}

				ImGui::SetCursorPos(endPos);

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::EndToolbar(ImVec2(0, 0));

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "BOX_LOGGER").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
					ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders
					))
				{
					static const float timestamp_width = ImGui::CalcTextSize("00:00:00:0000").x;

					ImGui::PushTextWrapPos();
					std::lock_guard<std::mutex> lock(MESSAGE_MUTEX);
					for (size_t i = 0; i < m_aFilteredMessages.size(); i++)
					{
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (m_Window.GetFramePadding().x * 2));

						auto& message = m_aMessages[m_aFilteredMessages[i]];

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (m_Window.GetFramePadding().x * 2));
						ImVec4 color = colors_arr[message.GetSeverity()];

						ImGui::PushFont(m_Window.GetIconFont());
						ImVec2 pos = ImGui::GetCursorPos();
						ImVec2 iconSize = ImGui::CalcTextSize(logo_arr[message.GetSeverity()].c_str());
						ImGui::TextColored(color, logo_arr[message.GetSeverity()].c_str());
						ImGui::PopFont();

						ImGui::SetCursorPos(ImVec2(
							pos.x + iconSize.x + (m_Window.GetFontSize() / 2),
							pos.y + (iconSize.y - ImGui::CalcTextSize(message.GetRawMessage().c_str()).y) * 0.5f));
						ImGui::TextUnformatted(message.GetRawMessage().c_str());

						time_t time_t = std::chrono::system_clock::to_time_t(message.GetTime());
						struct tm buf;

						localtime_s(&buf, &time_t);

						std::string s(30, '\0');
						std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", &buf);

						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().x);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (m_Window.GetFramePadding().x * 2));
						ImGui::TextColored(ImVec4(1, 1, 1, 0.5f), s.c_str());
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().x);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (m_Window.GetFramePadding().x * 2));
						ImGui::TextColored(ImVec4(1, 1, 1, 0.5f), message.GetLocation().generic_string().c_str());

						ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(message.GetCategory().c_str()).x);
						ImGui::PushFont(m_Window.GetBoldFont());
						ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive), message.GetCategory().c_str());
						ImGui::PopFont();

						if (i < m_aFilteredMessages.size() - 1)
						{
							ImGui::Separator();
						}

						if (scrollToBottom)
						{
							ImGui::SetScrollHereY(1.0f);
						}
					}

					ImGui::PopTextWrapPos();
				}
				ImGui::EndChild();
			}

			//---------------------------------------------------------------------
			bool ConsoleWindow::Initialize()
			{
				return BaseWindow::Initialize();
			}

			//---------------------------------------------------------------------
			bool ConsoleWindow::Destroy()
			{
				logger::LOGGER.OnMessageLogged() -= std::bind(&ConsoleWindow::LoggerCallback, this, std::placeholders::_1);
				return BaseWindow::Destroy();
			}

			//---------------------------------------------------------------------
			void ConsoleWindow::AddMessage(const logger::LoggerMessage& a_Message)
			{
				std::lock_guard<std::mutex> lock(MESSAGE_MUTEX);
				m_aMessages.push_back(a_Message);
				m_bNeedsRefresh = true;
			}

			//---------------------------------------------------------------------
			void ConsoleWindow::LoggerCallback(const logger::LoggerMessage& a_Message)
			{
				AddMessage(a_Message);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE