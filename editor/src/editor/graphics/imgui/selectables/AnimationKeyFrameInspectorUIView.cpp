#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "AnimationKeyFrameEditorSelectable.h"

// external
#include <typeindex>
#include <unordered_map>
#include <imgui/imgui_helpers.h>

// graphics
#include "graphics/imgui/font_icon.h"

// gameplay
#include "animation/AnimationKeyFrame.h"
#include "animation/Animation.h"

// utils
#include "utils/string_extensions.h"

// gameplay
#include "gameplay/Game.h"
#include "gameplay/ECSBaseSystem.h"
#include "gameplay/systems/AnimationSystem.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/RenderSerializableObject.h"

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
			AnimationKeyFrameEditorSelectable::AnimationKeyFrameEditorSelectable(ImGuiWindow& a_Window, animation::AnimationKeyFrame& a_KeyFrame, animation::Animation& a_Animation) : EditorSelectable(a_Window), m_KeyFrame(a_KeyFrame), m_Animation(a_Animation)
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
				m_Animation.RemoveKeyFrame(m_KeyFrame);
			}

			//---------------------------------------------------------------------
			bool AnimationKeyFrameEditorSelectable::RenderEditorFields()
			{
				bool changed = false;
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
					if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(font::ICON_DELETE, BUTTON_ID, id + "_DELETE_INSPECTOR").c_str(), "", size))
					{
						m_KeyFrame.RemoveComponent(component);

						changed = true;
					}

					ImGui::PopStyleVar();
					ImGui::PopStyleVar();

					if (m_aExpanded[id])
					{
						if (RenderObjectFields(component))
						{
							changed = true;
						}
					}
				}

				ImGui::SetCursorPosX(0 + m_Window.GetFramePadding().x);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FOLDER + std::string(" Add Component"), BUTTON_ID, "ADD_KEY_FRAME_COMPONENT_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					ImVec2 buttonPos = ImGui::GetItemRectMin();

					// Set the popup window position just below the button
					ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetItemRectSize().y));

					ImGui::OpenPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_KEY_FRAME_COMPONENT_MENU_INSPECTOR").c_str());
				}

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_Window.GetFramePadding());
				ImGui::SetNextWindowSize(ImVec2(width, 0));
				if (ImGui::BeginPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_KEY_FRAME_COMPONENT_MENU_INSPECTOR").c_str()))
				{
					for (auto* sys : core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::AnimationSystem>()->GetSystems())
					{
						bool found = false;
						for (animation::AnimationKeyFrameComponentBase* component : m_KeyFrame.GetComponents())
						{
							if (component->GetPropertyName() == sys->GetPropertyName())
							{
								found = true;
							}
						}
						if (!found && ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(sys->GetName(), MENU_ITEM_ID, "ADD_KEY_FRAME_COMPONENT_MENU_INSPECTOR" + sys->GetPropertyName()).c_str()))
						{
							m_KeyFrame.AddComponent(sys->CreateComponent(m_KeyFrame));
							changed = true;
						}
					}

					ImGui::EndPopup();
				}
				ImGui::PopStyleVar();

				ImGui::PopStyleVar();

				return changed;
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE