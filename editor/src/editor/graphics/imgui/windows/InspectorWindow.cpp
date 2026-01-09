#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "InspectorWindow.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/imgui/font_icon.h"

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
			InspectorWindow::InspectorWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_CIRCLE_INFO) + " Inspector", "Inspector"), m_NameInput(a_Window)
			{
				m_NameInput.Initialize("");
			}

			//---------------------------------------------------------------------
			bool InspectorWindow::Initialize()
			{
				PopulateToolbar();
				return BaseWindow::Initialize();
			}

			//---------------------------------------------------------------------
			bool InspectorWindow::Destroy()
			{
				return BaseWindow::Destroy();
			}

			//---------------------------------------------------------------------
			void InspectorWindow::PopulateToolbar()
			{
				ImVec2 toolbarSize = ImVec2(0, m_Window.GetHeaderSize().y);
				m_Toolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_INSPECTOR"), toolbarSize);

				// Open in explorer button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					[this]()
					{
						if (ImGui::TextButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_FOLDER_SHOW), BUTTON_ID, "SHOW_IN_EXPLORER_INSPECTOR").c_str(), "Opens the asset database or file explorer at the location of the selected object.", m_Window.GetHeaderSize()))
						{
							EditorSelectable* selectable = core::EDITOR_ENGINE->GetEditor().GetSelectable();
							selectable->OnShowInExplorer();
						}
					},
					[]()
					{
						EditorSelectable* selectable = core::EDITOR_ENGINE->GetEditor().GetSelectable();
						return !selectable->GetShowShowInExplorer();
					}
				));

				// Open in explorer button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					[this]()
					{
						if (ImGui::TextButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_DELETE), BUTTON_ID, "DELETE_INSPECTOR").c_str(), "Deletes the currently selected object from the scene or asset database.", m_Window.GetHeaderSize()))
						{
							m_bDeleteSelectable = true;
						}
					},
					[]()
					{
						EditorSelectable* selectable = core::EDITOR_ENGINE->GetEditor().GetSelectable();
						return !selectable->GetShowDelete();
					}
				));
			}

			//---------------------------------------------------------------------
			void InspectorWindow::DrawToolbar()
			{
				// Start toolbar.
				m_Toolbar.StartToolbar();

				// Render toolbar.
				m_Toolbar.Render();

				// End toolbar.
				m_Toolbar.EndToolbar();
			}

			//---------------------------------------------------------------------
			void InspectorWindow::Update()
			{
				if (core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetFullScreenPlayMode())
				{
					return;
				}
				BaseWindow::Update();
			}

			//---------------------------------------------------------------------
			void InspectorWindow::Render()
			{
				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				std::lock_guard<std::mutex> lock(core::EDITOR_ENGINE->GetEditor().m_EditorMutex);

				EditorSelectable* selectable = core::EDITOR_ENGINE->GetEditor().GetSelectable();
				if (!selectable)
				{
					return;
				}

				ImVec2 initialPos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(initialPos.x + m_Window.GetFramePadding().x, initialPos.y + m_Window.GetFramePadding().y));

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetWindowPadding());
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding()));

				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "INSPECTOR_TOOLBAR").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
					m_Window.GetHeaderSize().y
					),
					ImGuiChildFlags_None
					))
				{
					std::string icon = selectable->GetIcon();

					if (!icon.empty())
					{
						ImVec2 pos = ImGui::GetCursorPos();
						if (ImGui::BeginChild(
							ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "INSPECTOR_TOOLBAR_ICON").c_str(),
							m_Window.GetHeaderSize(),
							ImGuiChildFlags_None
							))
						{
							ImGui::PushFont(m_Window.GetBigIconFont());
							ImVec2 textSize = ImGui::CalcTextSize(icon.c_str());
							ImVec2 childSize = m_Window.GetHeaderSize();

							// Offset = half the difference between child height and text height
							float verticalOffset = (childSize.y - textSize.y) * 0.5f;
							float horizontalOffset = (childSize.x - textSize.x) * 0.5f;

							ImVec2 pos = ImGui::GetCursorPos();
							pos.y += verticalOffset;
							pos.x += horizontalOffset;

							ImGui::SetCursorPos(pos);
							ImGui::TextUnformatted(icon.c_str());

							ImGui::PopFont();
						}
						ImGui::EndChild();

						ImGui::SameLine();

						if (!selectable->GetShowRename())
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						}
						m_NameInput.SetString(selectable->GetName());
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (m_NameInput.Render(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "NAME_INSPECTOR").c_str(), ImGuiInputTextFlags_EnterReturnsTrue))
						{
							selectable->OnRename(m_NameInput.GetString());
						}
						if (!selectable->GetShowRename())
						{
							ImGui::PopItemFlag();
						}
					}
				}
				ImGui::EndChild();

				DrawToolbar();

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "INSPECTOR_VIEW").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
					(ImGui::GetContentRegionAvail().y) - m_Window.GetFramePadding().y),
					ImGuiChildFlags_Borders))
				{
					if (selectable->RenderEditorFields())
					{
						core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
					}
				}
				ImGui::EndChild();

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				if (m_bDeleteSelectable)
				{
					selectable->OnDelete();
				}
			}
		}
	}
}

#endif // IMGUI_DISABLE
#endif // _EDITOR