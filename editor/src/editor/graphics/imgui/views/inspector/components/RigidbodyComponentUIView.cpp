#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include <limits>

#include "RigidbodyComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_toggle.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// gameplay includes
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string RigidbodyComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void RigidbodyComponentUIView::RenderInner()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_Window.GetFramePadding());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetFramePadding());

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "Rigidbody_COMPONENT_TABLE_INSPECTOR"), m_Window.GetFramePadding());
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Gravity: ");
					},
					[this]
					{
						bool gravity = m_Component.UseGravity();
						if (ImGui::Toggle(ImGui::IMGUI_FORMAT_ID("", CHECKBOX_ID, "RIGIDBODY_COMPONENT_USE_GRAVITY_INSPECTOR").c_str(), &gravity))
						{
							m_Component.SetUseGravity(gravity);
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Mass: ");
					},
					[this]
					{
						float mass = m_Component.GetMass();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "RIGIDBODY_COMPONENT_MASS_INSPECTOR").c_str(), &mass, 0.5f, 0.0001f, std::numeric_limits<float>::max()))
						{
							m_Component.SetMass(mass);
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Linear Dampening: ");
					},
					[this]
					{
						float linearDamping = m_Component.GetLinearDamping();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "RIGIDBODY_COMPONENT_LINEAR_DAMPING_INSPECTOR").c_str(), &linearDamping, 0.1f, 0.0f, 1.0f))
						{
							m_Component.SetLinearDamping(linearDamping);
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE