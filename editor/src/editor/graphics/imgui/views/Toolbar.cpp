#include "Toolbar.h"

// external
#include <imgui/imgui_internal.h>

// graphics
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/imgui/font_icon.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// Toolbar
			//---------------------------------------------------------------------
			ToolbarButton::ToolbarButton(ImGuiWindow& a_Window, std::function<void()> a_ButtonFunc, std::function<bool()> a_DisabledCondition) : ToolbarItem(a_Window),
				m_ButtonFunc(a_ButtonFunc),
				m_DisabledCondition(a_DisabledCondition)
			{}

			//---------------------------------------------------------------------
			void ToolbarButton::Render()
			{
				bool wasDisabled = m_DisabledCondition();
				if (wasDisabled)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				m_ButtonFunc();
				if (wasDisabled)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}
			
			//---------------------------------------------------------------------
			ToolbarSearchbar::ToolbarSearchbar(ImGuiWindow& a_Window, const std::string& a_sID, float a_fWidth, std::function<void(const std::string&)> a_ButtonFunc) : ToolbarItem(a_Window),
				m_sID(a_sID), m_fWidth(a_fWidth), m_ButtonFunc(a_ButtonFunc)
			{
				m_Data = core::Data(256);
			}
			
			//---------------------------------------------------------------------
			void ToolbarSearchbar::Render()
			{
				ImVec2 initialScreenPos = ImGui::GetCursorScreenPos();

				ImVec2 padding = ImGui::GetStyle().FramePadding;

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (padding.x / 2.0f));

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding.x * 2.0f, padding.y / 2.0f));

				float regionHeight = ImGui::GetContentRegionAvail().y;
				float frameHeight = ImGui::GetFrameHeight();

				float cursorY = ImGui::GetCursorPosY();
				float offsetY = (regionHeight - frameHeight) * 0.5f;

				ImGui::SetCursorPosY(cursorY + offsetY);

				ImVec2 inputfieldPos = ImGui::GetCursorPos();

				ImGui::SetNextItemWidth(m_fWidth);
				bool success = ImGui::InputText(m_sID.c_str(), reinterpret_cast<char*>(m_Data.data()), m_Data.size());

				ImGui::PushFont(m_Window.GetSmallIconFont());
				ImVec2 searchIconSize = ImGui::CalcTextSize(font::ICON_SEARCH);
				float searchIconOffsetY = (frameHeight - searchIconSize.y) * 0.5f;

				ImGui::SetCursorPos(ImVec2(
					inputfieldPos.x + (padding.x / 2.0f),
					inputfieldPos.y + searchIconOffsetY
				));

				ImGui::Text(font::ICON_SEARCH);
				ImGui::PopFont();

				ImGui::PushFont(m_Window.GetSmallIconFont());
				ImVec2 closeIconSize = ImGui::CalcTextSize(font::ICON_CLOSE);
				float closeIconOffsetY = (frameHeight - closeIconSize.y) * 0.5f;

				ImGui::SetCursorPos(ImVec2(
					(inputfieldPos.x + m_fWidth) - (padding.x + (padding.x / 2.0f)),
					inputfieldPos.y + closeIconOffsetY
				));

				ImVec2 closeButtonScreenPos = ImGui::GetCursorScreenPos();
				ImGui::Text(font::ICON_CLOSE);
				ImGui::PopFont();

				ImVec2 clearButtonMin = closeButtonScreenPos;
				ImVec2 clearButtonMax = ImVec2(
					clearButtonMin.x + closeIconSize.x,
					clearButtonMin.y + closeIconSize.y
				);

				ImVec2 mousePos = ImGui::GetMousePos();

				bool isClearButtonHovered =
					mousePos.x >= clearButtonMin.x &&
					mousePos.x <= clearButtonMax.x &&
					mousePos.y >= clearButtonMin.y &&
					mousePos.y <= clearButtonMax.y;

				bool isClearButtonClicked = ImGui::IsMouseClicked(0) && isClearButtonHovered;

				if (isClearButtonHovered)
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				if (isClearButtonClicked)
				{
					ImGui::ClearActiveID();
					m_Data[0] = '\0';
					ImGui::SetKeyboardFocusHere(-1);
					success = true;
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::SetCursorScreenPos(initialScreenPos);
				ImGui::Dummy(ImVec2(m_fWidth + padding.x, frameHeight));

				if (success)
				{
					m_ButtonFunc(std::string(reinterpret_cast<char*>(m_Data.data())));
				}
			}

			//---------------------------------------------------------------------
			void ToolbarBreak::Render()
			{
				ImGui::Dummy(m_vBreakSize);
			}

			//---------------------------------------------------------------------
			void Toolbar::StartToolbar()
			{
				m_vToolbarSizeFinal = m_vToolbarSize;
				if (m_vToolbarSizeFinal.x == 0)
				{
					m_vToolbarSizeFinal.x = ImGui::GetContentRegionAvail().x;
				}
				if (m_vToolbarSizeFinal.y == 0)
				{
					m_vToolbarSizeFinal.y = ImGui::GetContentRegionAvail().y;
				}

				m_vToolbarStartScreenPos = ImGui::GetCursorScreenPos();
				m_vToolbarStartPos = ImGui::GetCursorPos();

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
			}

			//---------------------------------------------------------------------
			void Toolbar::EndToolbar()
			{
				ImGui::SetCursorScreenPos({
					m_vToolbarStartScreenPos.x,
					m_vToolbarStartScreenPos.y + m_vToolbarSize.y
					});

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			//---------------------------------------------------------------------
			void Toolbar::Render()
			{
				if (ImGui::BeginChild(m_sID.c_str(), m_vToolbarSizeFinal))
				{
					ImDrawList* draw_list = ImGui::GetWindowDrawList();

					ImVec2 header_min = m_vToolbarStartScreenPos;
					ImVec2 header_max = ImVec2(header_min.x + m_vToolbarSizeFinal.x, header_min.y + m_vToolbarSizeFinal.y);

					draw_list->AddRectFilled(header_min, header_max, ImGui::GetColorU32(ImGuiCol_Button), 0);

					for (size_t i = 0; i < m_aToolbarItems.size(); i++)
					{
						m_aToolbarItems[i]->Render();
						if (i < m_aToolbarItems.size() - 1)
						{
							ImGui::SameLine();
						}
					}
				}
				ImGui::EndChild();
			}
		}
	}
}