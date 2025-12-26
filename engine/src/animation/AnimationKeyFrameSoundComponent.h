#pragma once

#include "AnimationKeyFrameComponentBase.h"

#define ANIMATION_KEY_FRAME_SOUND_COMPONENT_NAME "Sound"
#define ANIMATION_KEY_FRAME_SOUND_COMPONENT_PROPERTY_NAME "sound"

namespace gallus
{
	namespace audio
	{
		class Sound;
	}
	namespace animation
	{
		class AnimationKeyFrameSoundComponent : public AnimationKeyFrameComponentBase
		{
		public:
			/// <summary>
			/// Constructs a key frame sprite component.
			/// </summary>
			/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
			AnimationKeyFrameSoundComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
			{
			}

			/// <summary>
			/// Activates the key frame component's behaviour.
			/// </summary>
			/// <param name="a_EntityID">The related entity id.</param>
			void Activate(gameplay::EntityID& a_EntityID) override
			{}

			std::string GetName() const override;

			const std::string GetPropertyName() const override
			{
				return ANIMATION_KEY_FRAME_SOUND_COMPONENT_PROPERTY_NAME;
			}
		protected:
			audio::Sound* m_pSound = nullptr;
		};

		class AnimationKeyFrameSoundSystem : public AnimationKeyFrameBaseSystem
		{
		public:
			std::string GetName() const override;

			const std::string GetPropertyName() const override
			{
				return ANIMATION_KEY_FRAME_SOUND_COMPONENT_PROPERTY_NAME;
			}

			AnimationKeyFrameComponentBase* CreateComponent(AnimationKeyFrame& a_KeyFrame) override
			{
				return new AnimationKeyFrameSoundComponent(a_KeyFrame);
			}
		};
	}
}