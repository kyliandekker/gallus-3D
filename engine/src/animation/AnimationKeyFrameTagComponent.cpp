#include "animation/AnimationKeyFrameTagComponent.h"

#include "graphics/imgui/font_icon.h"

namespace gallus
{
	namespace animation
	{
		//---------------------------------------------------------------------
		std::string AnimationKeyFrameTagComponent::GetName() const
		{
			std::string name = ANIMATION_KEY_FRAME_TAG_COMPONENT_NAME;
#ifdef _EDITOR
			name = std::string(font::ICON_TAG) + " " + name;
#endif // _EDITOR
			return name;
		}

		//---------------------------------------------------------------------
		std::string AnimationKeyFrameTagSystem::GetName() const
		{
			std::string name = ANIMATION_KEY_FRAME_TAG_COMPONENT_NAME;
#ifdef _EDITOR
			name = std::string(font::ICON_TAG) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}