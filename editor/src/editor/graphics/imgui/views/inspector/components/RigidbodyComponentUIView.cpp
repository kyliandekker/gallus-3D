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
				ImGuiWindow& window = m_Window;
				gameplay::RigidbodyComponent& rigidbodyComp = m_Component;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Gravity: ");
					},
					[&rigidbodyComp]
					{
						bool gravity = rigidbodyComp.UseGravity();
						if (ImGui::Toggle(ImGui::IMGUI_FORMAT_ID("", CHECKBOX_ID, "RIGIDBODY_COMPONENT_USE_GRAVITY_INSPECTOR").c_str(), &gravity))
						{
							rigidbodyComp.SetUseGravity(gravity);
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Mass: ");
					},
					[&rigidbodyComp]
					{
						float mass = rigidbodyComp.GetMass();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "RIGIDBODY_COMPONENT_MASS_INSPECTOR").c_str(), &mass, 0.5f, 0.0001f, std::numeric_limits<float>::max()))
						{
							rigidbodyComp.SetMass(mass);
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Linear Dampening: ");
					},
					[&rigidbodyComp]
					{
						float linearDamping = rigidbodyComp.GetLinearDamping();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "RIGIDBODY_COMPONENT_LINEAR_DAMPING_INSPECTOR").c_str(), &linearDamping, 0.1f, 0.0f, 1.0f))
						{
							rigidbodyComp.SetLinearDamping(linearDamping);
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