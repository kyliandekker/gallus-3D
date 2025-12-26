#pragma once

#include "AnimationKeyFrameComponentBase.h"

#define ANIMATION_KEY_FRAME_TAG_COMPONENT_NAME "Tag"
#define ANIMATION_KEY_FRAME_TAG_COMPONENT_PROPERTY_NAME "tag"

namespace gallus
{
	namespace animation
	{
		class Animation;

		class AnimationKeyFrameTagComponent : public AnimationKeyFrameComponentBase
		{
		public:
			/// <summary>
			/// Constructs a key frame sprite component.
			/// </summary>
			/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
			AnimationKeyFrameTagComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
			{
			}

			/// <summary>
			/// Activates the key frame component's behaviour.
			/// </summary>
			/// <param name="a_EntityID">The related entity id.</param>
			void Activate(gameplay::EntityID& a_EntityID) override
			{ }

			std::string GetName() const override;

			const std::string GetPropertyName() const override
			{
				return ANIMATION_KEY_FRAME_TAG_COMPONENT_PROPERTY_NAME;
			}
		private:
			std::string m_sTag;

			BEGIN_EXPOSE_FIELDS_PARENT(AnimationKeyFrameTagComponent, AnimationKeyFrameComponentBase)
			END_EXPOSE_FIELDS(AnimationKeyFrameTagComponent)
			BEGIN_EXPOSE_GIZMOS(AnimationKeyFrameTagComponent)
			END_EXPOSE_GIZMOS(AnimationKeyFrameTagComponent)
			END_EXPOSE_TO_EDITOR(AnimationKeyFrameTagComponent)
		};

		class AnimationKeyFrameTagSystem : public AnimationKeyFrameBaseSystem
		{
		public:
			std::string GetName() const override;

			const std::string GetPropertyName() const override
			{
				return ANIMATION_KEY_FRAME_TAG_COMPONENT_PROPERTY_NAME;
			}

			AnimationKeyFrameComponentBase* CreateComponent(AnimationKeyFrame& a_KeyFrame) override
			{
				return new AnimationKeyFrameTagComponent(a_KeyFrame);
			}
		};
	}
}