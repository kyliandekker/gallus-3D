#include "animation/AnimationKeyFrameEventComponent.h"

#include "graphics/imgui/font_icon.h"

// animation
#include "animation/AnimationKeyFrame.h"
#include "animation/Animation.h"

// resources
#include "resources/SrcData.h"

namespace gallus
{
	namespace animation
	{
		//---------------------------------------------------------------------
		void AnimationKeyFrameEventComponent::Activate(gameplay::EntityID& a_EntityID)
		{
			m_KeyFrame.GetAnimation()->ActivateEvent(a_EntityID, m_Event);
		}

		//---------------------------------------------------------------------
		std::string AnimationKeyFrameEventComponent::GetName() const
		{
			std::string name = ANIMATION_KEY_FRAME_EVENT_COMPONENT_NAME;
#ifdef _EDITOR
			name = std::string(font::ICON_EVENT) + " " + name;
#endif // _EDITOR
			return name;
		}

		//---------------------------------------------------------------------
		std::string AnimationKeyFrameEventSystem::GetName() const
		{
			std::string name = ANIMATION_KEY_FRAME_EVENT_COMPONENT_NAME;
#ifdef _EDITOR
			name = std::string(font::ICON_EVENT) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}