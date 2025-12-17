#pragma once

#include "gameplay/systems/components/Component.h"

#include <memory>

namespace gallus
{
	namespace animation
	{
		class AnimationTrack;
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

			void LoadAnimation(const std::string& a_sAnimName);

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

#ifdef _EDITOR
			/// <summary>
			/// Sets the starting animation.
			/// </summary>
			/// <param name="a_sStartingAnimation">The starting animation represented as a string.</param>
			void SetStartingAnimation(const std::string a_sStartingAnimation)
			{
				m_sStartingAnimation = a_sStartingAnimation;
			}
#endif
			/// <summary>
			/// Retrieves the starting animation.
			/// </summary>
			/// <returns>A string representing the starting animation.</returns>
			const std::string& GetStartingAnimation() const
			{
				return m_sStartingAnimation;
			}
		private:
			std::weak_ptr<animation::AnimationTrack> m_AnimationTrack = {};
			
			std::string m_sStartingAnimation;

			int m_iNextKeyFrameIndex = 0;
			bool m_bIsLooping = false;
			bool m_bIsPlaying = false;
			float m_fAccumulatedTime = 0.0f;

#ifdef _EDITOR	
			BEGIN_EXPOSE_FIELDS_PARENT(AnimationComponent, Component)
			END_EXPOSE_FIELDS(AnimationComponent)
			BEGIN_EXPOSE_GIZMOS(AnimationComponent)
			END_EXPOSE_GIZMOS(AnimationComponent)
			END_EXPOSE_TO_EDITOR(AnimationComponent)
#endif		  	
		};
	}
}