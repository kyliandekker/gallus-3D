#include "AnimationKeyFrameEventComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

#include "graphics/dx12/Texture.h"
#include "editor/graphics/imgui/EditorWindowsConfig.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			AnimationKeyFrameEventComponentUIView::AnimationKeyFrameEventComponentUIView(ImGuiWindow& a_Window, animation::AnimationKeyFrameEventComponent& a_EventComponent) : AnimationKeyFrameComponentUIView(a_Window, a_EventComponent),
				m_AnimationEventDropdown(m_Window)
			{
				m_AnimationEventDropdown.Initialize(
					m_Component.GetEvent(),
					{
						animation::AnimationEvent::AnimationEvent_None,
						animation::AnimationEvent::AnimationEvent_Delete
					},
					animation::AnimationEventToString
				);
				m_AnimationEventDropdown.SetValue(m_Component.GetEvent());
			}

			void AnimationKeyFrameEventComponentUIView::RenderInner()
			{
				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "EVENT_ANIMATION_KEY_FRAME_TABLE_INSPECTOR"), m_Window.GetFramePadding());

				ImGuiWindow& window = m_Window;
				ImGui::KeyValue([&window]
				{
					ImGui::AlignTextToFramePadding();
					ImGui::DisplayHeader(window.GetBoldFont(), "Event: ");
				},
					[this]
				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (m_AnimationEventDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "EVENT_ANIMATION_KEY_FRAME_EVENT").c_str()))
					{
						m_Component.SetEvent(m_AnimationEventDropdown.GetValue());
					}
				});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
			}

			std::string AnimationKeyFrameEventComponentUIView::GetName() const
			{
				return m_Component.GetName();
			}
		}
	}
}