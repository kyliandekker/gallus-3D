#pragma once

#include "gameplay/systems/components/Component.h"

#include <memory>

namespace gallus
{
	namespace animation
	{
		class Animation;
	}
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// AnimationComponent
		//---------------------------------------------------------------------
		class AnimationComponent : public Component
		{
		public:
			/// <summary>
			/// Initializes the component in runtime.
			/// </summary>
			void InitRealtime() override;

			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			/// <param name="a_UpdateTime">Which order this update is called in.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

			/// <summary>
			/// Resets the animation.
			/// </summary>
			void LoadAnimation();

			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			/// <param name="a_UpdateTime">Which order this update is called in.</param>
			void UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

			/// <summary>
			/// Starts the playback of the component.
			/// </summary>
			void Start()
			{
				m_bIsPlaying = true;
			}

			/// <summary>
			/// Stops the playback of the component.
			/// </summary>
			void Stop()
			{
				m_bIsPlaying = false;
			}
		private:
			std::weak_ptr<animation::Animation> m_pAnimation = {};
			
			std::weak_ptr<animation::Animation> m_pStartingAnimation;

			uint16_t m_iNextKeyFrameIndex = 0;
			bool m_bIsLooping = false;
			bool m_bIsPlaying = false;
			float m_fAccumulatedTime = 0.0f;

			BEGIN_SERIALIZE_PARENT(AnimationComponent, Component)
				SERIALIZE_FIELD(m_pStartingAnimation, "Starting Animation", "If set, the sprite will start with this animation.",
					.type = FieldSerializationType::FieldSerializationType_EngineResource,
					.assetType = resources::AssetType::Animation)
			END_SERIALIZE(AnimationComponent)
		};
	}
}