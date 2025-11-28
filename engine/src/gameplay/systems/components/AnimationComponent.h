#pragma once

#include "gameplay/systems/components/Component.h"

#include "animation/AnimationTrack.h"

namespace gallus
{
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

#ifdef _EDITOR
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;
#endif

			/// <summary>
			/// Deerializes sourcce data and creates an instance based on it.
			/// </summary>
			/// <param name="a_SrcData">The src data used for the component.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;

			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

			void LoadAnimation(const std::string& a_sAnimName);

			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
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
			void SetStartingAnimation(const std::string a_sStartingAnimation)
			{
				m_sStartingAnimation = a_sStartingAnimation;
			}
#endif

			const std::string& GetStartingAnimation() const
			{
				return m_sStartingAnimation;
			}
		private:
			animation::AnimationTrack m_AnimationTrack;
			
			std::string m_sStartingAnimation;
			std::string m_sAnimName;

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