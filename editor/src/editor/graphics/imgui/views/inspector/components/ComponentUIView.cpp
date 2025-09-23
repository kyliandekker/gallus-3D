#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <rapidjson/document.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "utils/string_extensions.h"
#include "gameplay/ECSBaseSystem.h"
#include "gameplay/systems/components/Component.h"
#include "core/Engine.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// ComponentBaseUIView
			//---------------------------------------------------------------------
			void ComponentBaseUIView::RenderBaseComponent(gameplay::Component& a_Component, gameplay::AbstractECSSystem& a_System)
			{
				ImVec2 size = m_Window.GetHeaderSize();

				float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
				width -= size.x;
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::FoldOutButton(
					ImGui::IMGUI_FORMAT_ID(
					std::string((m_bFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + GetName()),
					FOLDOUT_ID, string_extensions::StringToUpper(GetName()) + "_INSPECTOR"), &m_bFoldedOut, ImVec2(width, size.y));
				ImGui::SameLine();
				if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(font::ICON_DELETE, BUTTON_ID, string_extensions::StringToUpper(GetName()) + "_DELETE_HIERARCHY").c_str(), size, m_Window.GetIconFont()))
				{
					a_System.DeleteComponent(m_EntityID);
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				if (m_bFoldedOut)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetWindowPadding().y);

					ImGui::Indent(m_Window.GetFramePadding().x);
					RenderInner();
					ImGui::Unindent(m_Window.GetFramePadding().x);
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE