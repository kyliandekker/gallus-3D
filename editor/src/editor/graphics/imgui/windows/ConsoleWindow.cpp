#include "ConsoleWindow.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics
#include "imgui_system/font_icon.h"
#include "imgui_system/ImGuiSystem.h"

// utils
#include "utils/string_extensions.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/Editor.h"

namespace gallus::graphics::imgui
{
	/// <summary>
	/// Converts a log severity enumeration value to its corresponding icon.
	/// </summary>
	/// <param name="a_LogSeverity">The log severity to convert.</param>
	/// <returns>A string representing the icon.</returns>
	inline const std::string LogSeverityToIcon(LogSeverity a_LogSeverity)
	{
		switch (a_LogSeverity)
		{
			case LOGSEVERITY_ASSERT:
			{
				return font::ICON_CIRCLE_ASSERT;
			}
			case LOGSEVERITY_ERROR:
			{
				return font::ICON_CIRCLE_ERROR;
			}
			case LOGSEVERITY_WARNING:
			{
				return font::ICON_CIRCLE_WARNING;
			}
			case LOGSEVERITY_INFO:
			{
				return font::ICON_CIRCLE_INFO;
			}
			case LOGSEVERITY_TEST:
			{
				return font::ICON_CIRCLE_TEST;
			}
			case LOGSEVERITY_SUCCESS:
			{
				return font::ICON_CIRCLE_SUCCESS;
			}
			case LOGSEVERITY_INFO_SUCCESS:
			{
				return font::ICON_CIRCLE_INFO_SUCCESS;
			}
			case LOGSEVERITY_AWESOME:
			{
				return font::ICON_CIRCLE_AWESOME;
			}
		}
		return "";
	}

	//---------------------------------------------------------------------
	// ConsoleWindow
	//---------------------------------------------------------------------
	ConsoleWindow::ConsoleWindow(ImGuiSystem& a_System) : BaseWindow(a_System, 0, std::string(font::ICON_CONSOLE) + " Console", "Console")
	{
		// We want every log message. Not just the ones after ImGui has been initialized.
		GetLogger().OnMessageLogged() += std::bind(&ConsoleWindow::LoggerCallback, this, std::placeholders::_1);

		m_aMessageCount = std::vector<size_t>(LogSeverity::LOGSEVERITY_AWESOME + 1);
	}

	//---------------------------------------------------------------------
	ConsoleWindow::~ConsoleWindow()
	{}

	//---------------------------------------------------------------------
	bool ConsoleWindow::Initialize()
	{
		PopulateToolbar();

		return BaseWindow::Initialize();
	}

	//---------------------------------------------------------------------
	void ConsoleWindow::Update()
	{
		if (GetEditorEngine().GetEditor()->GetEditorSettings().GetEditorState() != editor::EditorState::EditorState_Workspace)
		{
			return;
		}
		BaseWindow::Update();
	}

	ImVec4 S_ICON_COLORS[8] =
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

	ImVec4 S_TEXT_COLORS[8] =
	{
		ImGui::ConvertColorsRgba(255, 0, 0, 255),
		ImGui::ConvertColorsRgba(255, 0, 0, 255),
		ImGui::ConvertColorsRgba(255, 216, 0, 255),
		ImGui::ConvertColorsRgba(255, 255, 255, 255),
		ImGui::ConvertColorsRgba(255, 255, 255, 255),
		ImGui::ConvertColorsRgba(76, 255, 0, 255),
		ImGui::ConvertColorsRgba(255, 255, 255, 255),
		ImGui::ConvertColorsRgba(255, 110, 220, 255),
	};

	//---------------------------------------------------------------------
	void ConsoleWindow::PopulateToolbar()
	{
		ImVec2 toolbarSize = ImVec2(0, m_System.GetHeaderSize().y);
		m_Toolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_ANIMATION"), toolbarSize);

		// Search bar.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarSearchbar(m_System,
			ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SEARCHBAR_CONSOLE"),
			200,
			[this](const std::string& a_sResult)
			{
				m_sSearchBarText = a_sResult;
				m_bNeedsRefresh = true;
			}
		));

		// Clear button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,

			[this]()
			{
				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_CLEAR), BUTTON_ID, "CLEAR_CONSOLE").c_str(), "Clears all log messages from the console, including filtered and unfiltered messages.", m_System.GetHeaderSize()))
				{
					std::lock_guard<std::mutex> lock(MESSAGE_MUTEX);
					m_aFilteredMessages.clear();
					m_aMessages.clear();

					CountMessages();
				}
			}
		));

		// Scroll to bottom button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,

			[this]()
			{
				editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();
				
				bool scrollToBottom = editorSettings.GetScrollToBottom();
				if (ImGui::CheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SCROLL_TO_BOTTOM) + " Scroll to bottom", BUTTON_ID, "SCROLL_TO_BOTTOM_CONSOLE").c_str(), &scrollToBottom, "Toggles automatic scrolling of the console view to always show the latest messages.", ImVec2(10 * m_System.GetFontSize(), m_System.GetHeaderSize().y)))
				{
					editorSettings.SetScrollToBottom(scrollToBottom);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}
			}
		));

		// Show info button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,

			[this]()
			{
				editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();

				bool showInfo = editorSettings.GetShowInfo();
				if (ImGui::ConsoleButton(
					ImGui::IMGUI_FORMAT_ID(LogSeverityToIcon(LOGSEVERITY_INFO).c_str(), BUTTON_ID, "SHOW_INFO_CONSOLE").c_str(), &showInfo, "Toggles visibility of informational log messages.", m_System.GetHeaderSize(), m_aMessageCount[LOGSEVERITY_INFO], S_ICON_COLORS[LOGSEVERITY_INFO]))
				{
					editorSettings.SetShowInfo(showInfo);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}
			}
		));

		// Show test button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,

			[this]()
			{
				editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();

				bool showTest = editorSettings.GetShowTest();
				if (ImGui::ConsoleButton(
					ImGui::IMGUI_FORMAT_ID(LogSeverityToIcon(LOGSEVERITY_TEST).c_str(), BUTTON_ID, "SHOW_TEST_CONSOLE").c_str(), &showTest, "Toggles visibility of test log messages used for quick testing.", m_System.GetHeaderSize(), m_aMessageCount[LOGSEVERITY_TEST], S_ICON_COLORS[LOGSEVERITY_TEST]))
				{
					editorSettings.SetShowTest(showTest);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}
			}
		));

		// Show warning button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,

			[this]()
			{
				editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();

				bool showWarning = editorSettings.GetShowWarning();
				if (ImGui::ConsoleButton(
					ImGui::IMGUI_FORMAT_ID(LogSeverityToIcon(LOGSEVERITY_WARNING).c_str(), BUTTON_ID, "SHOW_WARNING_CONSOLE").c_str(), &showWarning, "Toggles visibility of warning log messages, indicating potential issues.", m_System.GetHeaderSize(), m_aMessageCount[LOGSEVERITY_WARNING], S_ICON_COLORS[LOGSEVERITY_WARNING]))
				{
					editorSettings.SetShowWarning(showWarning);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}
			}
		));

		// Show error button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,

			[this]()
			{
				editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();

				bool showError = editorSettings.GetShowError();
				if (ImGui::ConsoleButton(
					ImGui::IMGUI_FORMAT_ID(LogSeverityToIcon(LOGSEVERITY_ERROR).c_str(), BUTTON_ID, "SHOW_ERROR_CONSOLE").c_str(), &showError, "Toggles visibility of error log messages, indicating runtime or critical errors.", m_System.GetHeaderSize(), m_aMessageCount[LOGSEVERITY_ERROR], S_ICON_COLORS[LOGSEVERITY_ERROR], S_ICON_COLORS[LOGSEVERITY_ASSERT]))
				{
					editorSettings.SetShowError(showError);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}
			}
		));

		// Show assert button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,

			[this]()
			{
				editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();

				bool showAssert = editorSettings.GetShowAssert();
				if (ImGui::ConsoleButton(
					ImGui::IMGUI_FORMAT_ID(LogSeverityToIcon(LOGSEVERITY_ASSERT).c_str(), BUTTON_ID, "SHOW_ASSERT_CONSOLE").c_str(), &showAssert, "Toggles visibility of assert messages, typically generated by failed assertions in code.", m_System.GetHeaderSize(), m_aMessageCount[LOGSEVERITY_ASSERT], S_ICON_COLORS[LOGSEVERITY_ASSERT], S_ICON_COLORS[LOGSEVERITY_ASSERT]))
				{
					editorSettings.SetShowAssert(showAssert);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}
			}
		));

		// Show success button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,

			[this]()
			{
				editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();

				bool showSuccess = editorSettings.GetShowSuccess();
				if (ImGui::ConsoleButton(
					ImGui::IMGUI_FORMAT_ID(LogSeverityToIcon(LOGSEVERITY_SUCCESS).c_str(), BUTTON_ID, "SHOW_SUCCESS_CONSOLE").c_str(), &showSuccess, "Toggles visibility of success log messages, often used for visual confirmation of events.", m_System.GetHeaderSize(), m_aMessageCount[LOGSEVERITY_SUCCESS], S_ICON_COLORS[LOGSEVERITY_SUCCESS]))
				{
					editorSettings.SetShowSuccess(showSuccess);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}
			}
		));

		// Show info success button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,

			[this]()
			{
				editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();

				bool showInfoSuccess = editorSettings.GetShowInfoSuccess();
				if (ImGui::ConsoleButton(
					ImGui::IMGUI_FORMAT_ID(LogSeverityToIcon(LOGSEVERITY_INFO_SUCCESS).c_str(), BUTTON_ID, "SHOW_INFO_SUCCESS_CONSOLE").c_str(), &showInfoSuccess, "Toggles visibility of informational success messages, combining info and success levels. Often used between informational and success log messages.", m_System.GetHeaderSize(), m_aMessageCount[LOGSEVERITY_INFO_SUCCESS], S_ICON_COLORS[LOGSEVERITY_INFO_SUCCESS]))
				{
					editorSettings.SetShowInfoSuccess(showInfoSuccess);
					editorSettings.Save();
					m_bNeedsRefresh = true;
				}
			}
		));
	}

	//---------------------------------------------------------------------
	void ConsoleWindow::DrawToolbar()
	{
		// Start toolbar.
		m_Toolbar.StartToolbar();

		// Render toolbar.
		m_Toolbar.Render();

		// End toolbar.
		m_Toolbar.EndToolbar();
	}

	//---------------------------------------------------------------------
	void ConsoleWindow::Render()
	{
		editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();

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

			std::string searchString = string_extensions::StringToLower(m_sSearchBarText);
			for (size_t i = 0; i < m_aMessages.size(); i++)
			{
				const logger::LoggerMessage& message = m_aMessages[i];
				if (filters[message.GetSeverity()])
				{
					if (searchString.empty() || ((string_extensions::StringToLower(message.GetRawMessage()).find(searchString) != std::string::npos) || (string_extensions::StringToLower(message.GetCategory()).find(searchString) != std::string::npos)))
					{
						m_aFilteredMessages.push_back(i);
					}
				}
			}
		}

		DrawToolbar();

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_System.GetFramePadding().x, ImGui::GetCursorPos().y + m_System.GetFramePadding().y));
		if (ImGui::BeginChild(
			ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "BOX_CONSOLE").c_str(),
			ImVec2(
			ImGui::GetContentRegionAvail().x - m_System.GetFramePadding().x,
			ImGui::GetContentRegionAvail().y - m_System.GetFramePadding().y
			),
			ImGuiChildFlags_Borders
		))
		{
			static const float timestamp_width = ImGui::CalcTextSize("00:00:00:0000").x;

			ImGui::PushTextWrapPos();
			std::lock_guard<std::mutex> lock(MESSAGE_MUTEX);
			for (size_t i = 0; i < m_aFilteredMessages.size(); i++)
			{
				ImVec2 startingPos = ImGui::GetCursorScreenPos();

				const logger::LoggerMessage& message = m_aMessages[m_aFilteredMessages[i]];
				ImVec4 color = S_ICON_COLORS[message.GetSeverity()];

				time_t time_t = std::chrono::system_clock::to_time_t(message.GetTime());
				std::string id = ImGui::IMGUI_FORMAT_ID("",
					FOLDOUT_ID, string_extensions::StringToUpper(std::to_string(i) + "_" + std::to_string(time_t) + "_" + message.GetCategory() + "_MESSAGE_CONSOLE"));

				ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, m_System.GetFontSize() + (m_System.GetFramePadding().y * 2));
				if (ImGui::InvisibleButton(id.c_str(), buttonSize))
				{
					m_aExpanded[id] = !m_aExpanded[id];
				}

				ImGui::SetCursorScreenPos(startingPos + m_System.GetFramePadding());

				std::string foldoutIcon = m_aExpanded[id] ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN;

				ImVec2 pos = ImGui::GetCursorScreenPos();
				std::string icon = LogSeverityToIcon(message.GetSeverity());
				ImVec2 iconSize = ImGui::CalcTextSize(icon.c_str());

				ImGui::TextUnformatted(foldoutIcon.c_str());

				pos.x += m_System.GetFontSize();
				ImGui::SetCursorScreenPos(pos);

				ImGui::TextColored(color, icon.c_str());

				ImGui::SetCursorScreenPos(ImVec2(
					pos.x + iconSize.x + (m_System.GetFontSize() / 2),
					pos.y + (iconSize.y - ImGui::CalcTextSize(message.GetRawMessage().c_str()).y) * 0.5f));
				ImGui::TextColored(S_TEXT_COLORS[message.GetSeverity()], message.GetRawMessage().c_str());

				if (m_aExpanded[id])
				{
					ImGui::Indent();

					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

					struct tm buf;

					localtime_s(&buf, &time_t);

					std::string s(30, '\0');
					std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", &buf);

					ImGui::TextColored(ImVec4(1, 1, 1, 0.5f), s.c_str());

					std::string filePath = message.GetLocation();
					std::string line = std::to_string(message.GetLine());
					std::string location = filePath + " on line " + line;
					ImVec2 fileTextSize = ImGui::CalcTextSize(location.c_str());
					
					ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
					if (ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, message.GetLocation() + std::to_string(message.GetLine())).c_str(), fileTextSize))
					{
						std::string cmd = "code -g \"" + filePath + "\":" + line;
						system(cmd.c_str());
					}
					ImGui::SetCursorScreenPos(cursorScreenPos);
					ImGui::TextColored(ImVec4(1, 1, 1, 0.5f), location.c_str());

					ImGui::PopStyleVar();

					ImGui::Unindent();
				}

				if (editorSettings.GetScrollToBottom())
				{
					ImGui::SetScrollHereY(1.0f);
				}
			}

			ImGui::PopTextWrapPos();
		}
		ImGui::EndChild();
	}

	//---------------------------------------------------------------------
	bool ConsoleWindow::Destroy()
	{
		GetLogger().OnMessageLogged() -= std::bind(&ConsoleWindow::LoggerCallback, this, std::placeholders::_1);
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
		CountMessages();
	}
	
	//---------------------------------------------------------------------
	void ConsoleWindow::CountMessages()
	{
		m_aMessageCount.clear();
		m_aMessageCount = std::vector<size_t>(LogSeverity::LOGSEVERITY_AWESOME + 1);
		for (size_t i = 0; i < m_aMessages.size(); i++)
		{
			m_aMessageCount[m_aMessages[i].GetSeverity()] = 0;
		}
		for (size_t i = 0; i < m_aMessages.size(); i++)
		{
			m_aMessageCount[m_aMessages[i].GetSeverity()]++;
		}
	}
}