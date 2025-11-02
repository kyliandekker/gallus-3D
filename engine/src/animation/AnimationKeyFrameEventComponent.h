#pragma once

#include "AnimationKeyFrameComponentBase.h"

#include "animation/AnimationEvents.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
		}
	}
	namespace resources
	{
		class SrcData;
	}
	namespace animation
	{
		class AnimationKeyFrameEventComponent : public AnimationKeyFrameComponentBase
		{
		public:
			AnimationKeyFrameEventComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
			{
			}

			void Activate(gameplay::EntityID& a_EntityID, AnimationTrack& a_AnimationTrack) override;

			std::string GetName() const override
			{
				return "Event";
			}
#ifdef _EDITOR
			void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const override;
			std::string GetPropertyName() const override
			{
				return "eventComponent";
			}
#endif
			void Deserialize(const resources::SrcData& a_SrcData) override;

			AnimationEvent GetEvent() const
			{
				return m_Event;
			}

			void SetEvent(AnimationEvent a_Event)
			{
				m_Event = a_Event;
			}
		private:
			AnimationEvent m_Event
				;
		};
	}
}