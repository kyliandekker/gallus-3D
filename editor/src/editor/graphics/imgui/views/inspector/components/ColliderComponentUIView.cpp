#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ColliderComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// gameplay includes
#include "gameplay/Game.h"
#include "gameplay/systems/TransformSystem.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string ColliderComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void ColliderComponentUIView::RenderInner()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_Window.GetFramePadding());
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetFramePadding());

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Offset: ");
				ImGui::Indent();

				m_OffsetView.SetValue(m_Component.GetOffset());
				if (m_OffsetView.Render("COLLIDER_OFFSET_INSPECTOR", 0.01f))
				{
					m_Component.SetOffset(m_OffsetView.GetValue());
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Size: ");
				ImGui::Indent();

				m_SizeView.SetValue(m_Component.GetSize());
				if (m_SizeView.Render("COLLIDER_SIZE_INSPECTOR", 0.01f))
				{
					m_Component.SetSize(m_SizeView.GetValue());
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

            constexpr float HANDLE_RADIUS = 6;
            void ColliderComponentUIView::RenderComponentGizmos(
                const ImVec2& a_vScenePos,
                const ImVec2& a_vSize,
                const ImVec2& a_vPanOffset,
                float a_fZoom)
            {
            }
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE