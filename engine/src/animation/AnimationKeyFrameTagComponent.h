#pragma once

#include "AnimationKeyFrameComponentBase.h"

#define ANIMATION_KEY_FRAME_TAG_COMPONENT_NAME "Tag"
#define ANIMATION_KEY_FRAME_TAG_COMPONENT_PROPERTY_NAME "tag"

namespace gallus::animation
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
		{}

		/// <summary>
		/// Returns the name of the component.
		/// </summary>
		/// <returns>A string containing the name of the component.</returns>
		std::string GetName() const override;

		/// <summary>
		/// Returns the property name of the component.
		/// </summary>
		/// <returns>A string containing the name of the component used in serialization.</returns>
		const std::string GetPropertyName() const override
		{
			return ANIMATION_KEY_FRAME_TAG_COMPONENT_PROPERTY_NAME;
		}
	private:
		std::string m_sTag;

		BEGIN_SERIALIZE_PARENT(AnimationKeyFrameTagComponent, AnimationKeyFrameComponentBase)
		END_SERIALIZE(AnimationKeyFrameTagComponent)
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