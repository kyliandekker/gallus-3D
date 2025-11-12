#include "AnimationKeyFrameComponentBaseUIView.h"

#include <imgui/imgui_helpers.h>

#include "utils/string_extensions.h"

#include "graphics/imgui/font_icon.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			void AnimationKeyFrameComponentBaseUIView::RenderBaseComponent(animation::AnimationKeyFrameComponentBase& a_Component)
			{
				ImVec2 size = m_Window.GetHeaderSize();

				float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
				width -= size.x;
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::FoldOutButton(
					ImGui::IMGUI_FORMAT_ID(
						std::string((m_bFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + GetName()),
						FOLDOUT_ID, string_extensions::StringToUpper(GetName()) + "_ANIMATION_COMPONENT"), &m_bFoldedOut, ImVec2(width, size.y));
				ImGui::SameLine();
				if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(font::ICON_DELETE, BUTTON_ID, string_extensions::StringToUpper(GetName()) + "_ANIMATION_COMPONENT").c_str(), size, m_Window.GetIconFont()))
				{
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				if (m_bFoldedOut)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetWindowPadding().y);

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x, 0));
					ImGui::Indent(m_Window.GetFramePadding().x * 4);
					RenderInner();
					ImGui::Unindent(m_Window.GetFramePadding().x * 4);
					ImGui::PopStyleVar();
				}
			}
		}
	}
}
