#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "AnimationKeyFrameInspectorUIView.h"

#include <typeindex>
#include <unordered_map>
#include <imgui/imgui_helpers.h>

// core includes
#include "editor/core/EditorEngine.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

#include "gameplay/Game.h"
#include "gameplay/ECSBaseSystem.h"

#include "AnimationKeyFrameComponentBaseUIView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// AnimationKeyFrameInspectorUIView
			//---------------------------------------------------------------------
			AnimationKeyFrameInspectorUIView::~AnimationKeyFrameInspectorUIView()
			{
				for (AnimationKeyFrameComponentBaseUIView* view : m_aComponents)
				{
					delete view;
				}
			}

			//---------------------------------------------------------------------
			AnimationKeyFrameInspectorUIView::AnimationKeyFrameInspectorUIView(ImGuiWindow& a_Window, animation::AnimationKeyFrame& a_KeyFrame) : InspectorView(a_Window), m_KeyFrame(a_KeyFrame)
			{
				m_bShowDelete = true;

				m_aComponents.clear();
				for (auto& [type, factory] : GetKeyFrameComponentUIFactoryRegistry())
				{
					if (AnimationKeyFrameComponentBaseUIView* view = factory(m_Window, m_KeyFrame))
					{
						m_aComponents.push_back(view);
					}
				}
			}

			//---------------------------------------------------------------------
			std::string AnimationKeyFrameInspectorUIView::GetName() const
			{
				return "Animation Track";
			}

			//---------------------------------------------------------------------
			std::string AnimationKeyFrameInspectorUIView::GetIcon() const
			{
				return font::ICON_ANIMATION;
			}

			//---------------------------------------------------------------------
			void AnimationKeyFrameInspectorUIView::Render()
			{
				float width = ImGui::GetContentRegionAvail().x;
				
				for (auto* comp : m_aComponents)
				{
					comp->Render();
				}

				//if (!m_KeyFrame.HasComponent<animation::AnimationKeyFrameSpriteComponent>())
				//{
				//	if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_IMAGE + std::string(" Add Sprite Component"), BUTTON_ID, "ADD_SPRITE_COMPONENT_ANIMATION_MODAL").c_str(), ImVec2(width, 0)))
				//	{
				//		keyFrame.AddComponent<animation::AnimationKeyFrameSpriteComponent>();
				//		m_AnimationTrackUIView.GetKeyFrameUIView().SetKeyFrame(keyFrame);
				//	}
				//}
				//if (!m_KeyFrame.HasComponent<animation::AnimationKeyFrameEventComponent>())
				//{
				//	if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_CIRCLE_ERROR + std::string(" Add Event Component"), BUTTON_ID, "ADD_EVENT_COMPONENT_ANIMATION_MODAL").c_str(), ImVec2(width, 0)))
				//	{
				//		keyFrame.AddComponent<animation::AnimationKeyFrameEventComponent>();
				//		m_AnimationTrackUIView.GetKeyFrameUIView().SetKeyFrame(keyFrame);
				//	}
				//}
			}

			//---------------------------------------------------------------------
			void AnimationKeyFrameInspectorUIView::RenderPreview()
			{
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE