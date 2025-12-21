#include "animation/AnimationKeyFrameEventComponent.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/CommandQueue.h"

#include "graphics/imgui/font_icon.h"

// animation
#include "animation/AnimationKeyFrame.h"
#include "animation/Animation.h"

// resources
#include "resources/SrcData.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace animation
	{
		//---------------------------------------------------------------------
		void AnimationKeyFrameEventComponent::Activate(gameplay::EntityID& a_EntityID)
		{
			m_KeyFrame.GetAnimation()->ActivateEvent(a_EntityID, m_Event);
		}

#ifdef _EDITOR
		//---------------------------------------------------------------------
		void AnimationKeyFrameEventComponent::Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator)  const
		{
			int event = (int) m_Event;
			a_Value.AddMember(
				"event",
				event,
				a_Allocator
			);
		}
#endif _EDITOR

		//---------------------------------------------------------------------
		void AnimationKeyFrameEventComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			if (!a_SrcData.GetEnum<AnimationEvent>("event", m_Event))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Animation event component did not have key %s present in its meta data.", "event");
			}
		}

		//---------------------------------------------------------------------
		std::string AnimationKeyFrameEventComponent::GetName() const
		{
			std::string name = "Event";
#ifdef _EDITOR
			name = std::string(font::ICON_EVENT) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}