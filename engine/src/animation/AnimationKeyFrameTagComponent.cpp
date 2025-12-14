#include "animation/AnimationKeyFrameTagComponent.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/CommandQueue.h"

#include "graphics/imgui/font_icon.h"

// resources
#include "resources/SrcData.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace animation
	{
		//---------------------------------------------------------------------
		void AnimationKeyFrameTagComponent::Activate(gameplay::EntityID& a_EntityID)
		{
		}

#ifdef _EDITOR
		//---------------------------------------------------------------------
		void AnimationKeyFrameTagComponent::Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator)  const
		{
			a_Value.AddMember(
				"tag",
				rapidjson::Value(m_sTag.c_str(), a_Allocator),
				a_Allocator
			);
		}
#endif _EDITOR
		//---------------------------------------------------------------------
		void AnimationKeyFrameTagComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			if (!a_SrcData.GetString("tag", m_sTag))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "animation tag component did not have key %s present in its meta data.", "tag");
			}
		}

		//---------------------------------------------------------------------
		std::string AnimationKeyFrameTagComponent::GetName() const
		{
			std::string name = "Tag";
#ifdef _EDITOR
			name = std::string(font::ICON_TAG) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}