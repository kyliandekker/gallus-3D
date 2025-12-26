#include "animation/AnimationKeyFrameSoundComponent.h"

#include "graphics/imgui/font_icon.h"

// resources
#include "resources/SrcData.h"

namespace gallus
{
	namespace animation
	{
		//---------------------------------------------------------------------
		std::string AnimationKeyFrameSoundComponent::GetName() const
		{
			std::string name = ANIMATION_KEY_FRAME_SOUND_COMPONENT_NAME;
#ifdef _EDITOR
			name = std::string(font::ICON_FILE_AUDIO) + " " + name;
#endif // _EDITOR
			return name;
		}

		//---------------------------------------------------------------------
		std::string AnimationKeyFrameSoundSystem::GetName() const
		{
			std::string name = ANIMATION_KEY_FRAME_SOUND_COMPONENT_NAME;
#ifdef _EDITOR
			name = std::string(font::ICON_FILE_AUDIO) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}