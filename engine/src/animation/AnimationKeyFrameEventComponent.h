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
			/// Retrieves the name of the key frame component.
			/// </summary>
			/// <returns>A string containing the name.</returns>
			std::string GetName() const override;

#ifdef _EDITOR
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const override;
#endif
			/// <summary>
			/// Retrieves the property name of the key frame component.
			/// </summary>
			/// <returns>A string containing the property name.</returns>
			std::string GetPropertyName() const override
			{
				return "event";
			}

			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;
#ifdef _EDITOR
			/// <summary>
			/// Retrieves the event.
			/// </summary>
			/// <returns>The event attached to this key frame.</returns>
			AnimationEvent GetEvent() const
			{
				return m_Event;
			}

			/// <summary>
			/// Sets the event used in this key frame.
			/// </summary>
			/// <param name="a_Event">The event.</param>
			void SetEvent(AnimationEvent a_Event)
			{
				m_Event = a_Event;
			}
#endif
		private:
			AnimationEvent m_Event;

#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS(AnimationKeyFrameEventComponent)
				EXPOSE_ENUM_FIELD_AUTO(AnimationKeyFrameEventComponent, m_Event, "Event Type", AnimationEvent)
			END_EXPOSE_FIELDS(AnimationKeyFrameEventComponent)
			BEGIN_EXPOSE_GIZMOS(AnimationKeyFrameEventComponent)
			END_EXPOSE_GIZMOS(AnimationKeyFrameEventComponent)
			END_EXPOSE_TO_EDITOR(AnimationKeyFrameEventComponent)
#endif
		};
	}
}