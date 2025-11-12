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

#ifdef _EDITOR
			void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const override;
#endif
			std::string GetName() const override;

			std::string GetPropertyName() const override
			{
				return "event";
			}

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
			AnimationEvent m_Event;

			BEGIN_EXPOSED_FIELDS(AnimationKeyFrameEventComponent)
				EXPOSE_ENUM_FIELD_AUTO(AnimationKeyFrameEventComponent, m_Event, "Event Type", AnimationEvent)
			END_EXPOSED_FIELDS(AnimationKeyFrameEventComponent)
		};
	}
}