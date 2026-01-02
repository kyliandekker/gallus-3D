#pragma once

#include "AnimationKeyFrameComponentBase.h"

// animation
#include "animation/AnimationEvents.h"

#define ANIMATION_KEY_FRAME_EVENT_COMPONENT_NAME "Event"
#define ANIMATION_KEY_FRAME_EVENT_COMPONENT_PROPERTY_NAME "event"

namespace gallus
{
	namespace animation
	{
		class AnimationKeyFrameEventComponent : public AnimationKeyFrameComponentBase
		{
		public:
			/// <summary>
			/// Constructs a key frame event component.
			/// </summary>
			/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
			AnimationKeyFrameEventComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
			{
			}

			/// <summary>
			/// Activates the key frame component's behaviour.
			/// </summary>
			/// <param name="a_EntityID">The related entity id.</param>
			void Activate(gameplay::EntityID& a_EntityID) override;

			/// <summary>
			/// Retrieves the event.
			/// </summary>
			/// <returns>The event attached to this key frame.</returns>
			AnimationEvent GetEvent() const
			{
				return m_Event;
			}

			std::string GetName() const override;

			const std::string GetPropertyName() const override
			{
				return ANIMATION_KEY_FRAME_EVENT_COMPONENT_PROPERTY_NAME;
			}
		private:
			AnimationEvent m_Event;

			BEGIN_SERIALIZE_PARENT(AnimationKeyFrameEventComponent, AnimationKeyFrameComponentBase)
			END_SERIALIZE(AnimationKeyFrameEventComponent)
		};

		class AnimationKeyFrameEventSystem : public AnimationKeyFrameBaseSystem
		{
		public:
			std::string GetName() const override;

			const std::string GetPropertyName() const override
			{
				return ANIMATION_KEY_FRAME_EVENT_COMPONENT_PROPERTY_NAME;
			}

			AnimationKeyFrameComponentBase* CreateComponent(AnimationKeyFrame& a_KeyFrame) override
			{
				return new AnimationKeyFrameEventComponent(a_KeyFrame);
			}
		};
	}
}