#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "AnimationKeyFrameEditorSelectable.h"

#include <typeindex>
#include <unordered_map>
#include <imgui/imgui_helpers.h>

// core includes
#include "editor/core/EditorEngine.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

#include "gameplay/Game.h"
#include "gameplay/ECSBaseSystem.h"

#include "editor/graphics/imgui/RenderEditorExposable.h"

#include "utils/string_extensions.h"

#include "animation/AnimationKeyFrame.h"
#include "animation/AnimationTrack.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// AnimationKeyFrameEditorSelectable
			//---------------------------------------------------------------------
			AnimationKeyFrameEditorSelectable::~AnimationKeyFrameEditorSelectable()
			{
			}

			//---------------------------------------------------------------------
			AnimationKeyFrameEditorSelectable::AnimationKeyFrameEditorSelectable(ImGuiWindow& a_Window, animation::AnimationKeyFrame& a_KeyFrame, animation::AnimationTrack& a_AnimationTrack) : EditorSelectable(a_Window), m_KeyFrame(a_KeyFrame), m_AnimationTrack(a_AnimationTrack)
			{
				m_bShowDelete = true;
			}

			//---------------------------------------------------------------------
			std::string AnimationKeyFrameEditorSelectable::GetName() const
			{
				return "Animation Track";
			}

			//---------------------------------------------------------------------
			std::string AnimationKeyFrameEditorSelectable::GetIcon() const
			{
				return font::ICON_ANIMATION;
			}

			//---------------------------------------------------------------------
			void AnimationKeyFrameEditorSelectable::OnDelete()
			{
				m_AnimationTrack.RemoveKeyFrame(m_KeyFrame);
			}

			//---------------------------------------------------------------------
			void AnimationKeyFrameEditorSelectable::Render()
			{
				ImGui::SetCursorPosY(0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

				for (animation::AnimationKeyFrameComponentBase* component : m_KeyFrame.GetComponents())
				{
					ImGui::SetCursorPosX(0);
					float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
					ImGui::SetNextItemWidth(width);

					ImVec2 size = m_Window.GetHeaderSize();

					width -= size.x;
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

					ImVec2 foldOutButtonPos = ImGui::GetCursorScreenPos();

					std::string id = ImGui::IMGUI_FORMAT_ID("",
						FOLDOUT_ID, string_extensions::StringToUpper(component->GetTypeName()) + "_INSPECTOR");

					ImGui::FoldOutButton(
						std::string((m_aExpanded[id] ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(component->GetName()) + id).c_str(), &m_aExpanded[id], ImVec2(width, size.y));
					ImGui::SameLine();
					if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(font::ICON_DELETE, BUTTON_ID, id + "_DELETE_INSPECTOR").c_str(), size, m_Window.GetIconFont()))
					{
						m_KeyFrame.RemoveComponent(component);

						m_KeyFrame.GetAnimationTrack()->SetIsDirty(true);
					}

					ImGui::PopStyleVar();
					ImGui::PopStyleVar();

					if (m_aExpanded[id])
					{
						if (RenderObjectFields(component))
						{
							m_KeyFrame.GetAnimationTrack()->SetIsDirty(true);
						}
					}
				}

				ImGui::PopStyleVar();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE