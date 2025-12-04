#include "animation/AnimationKeyFrameSoundComponent.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/CommandQueue.h"

#include "graphics/imgui/font_icon.h"

// resources
#include "resources/SrcData.h"

namespace gallus
{
	namespace animation
	{
		//---------------------------------------------------------------------
		void AnimationKeyFrameSoundComponent::Deserialize(const resources::SrcData& a_SrcData)
		{}

#ifdef _EDITOR
		//---------------------------------------------------------------------
		void AnimationKeyFrameSoundComponent::Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator)  const
		{
		}
#endif _EDITOR

		//---------------------------------------------------------------------
		std::string AnimationKeyFrameSoundComponent::GetName() const
		{
			std::string name = "Sound";
#ifdef _EDITOR
			name = std::string(font::ICON_FILE_AUDIO) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}