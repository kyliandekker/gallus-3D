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

			void LoadAnimation(const std::string& a_sAnimName);

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

			int m_iNextKeyFrameIndex = 0;
			bool m_bIsLooping = false;
			bool m_bIsPlaying = false;
			float m_fAccumulatedTime = 0.0f;

			BEGIN_EXPOSE_FIELDS_PARENT(AnimationComponent, Component)
				EXPOSE_FIELD(AnimationComponent, m_pStartingAnimation, "Starting Animation", (FieldOptions{ .type = EditorFieldWidgetType::EditorFieldWidgetType_EngineResource, .assetType = resources::AssetType::Animation, .description = "If set, the sprite will start with this animation." }))
			END_EXPOSE_FIELDS(AnimationComponent)
			BEGIN_EXPOSE_GIZMOS(AnimationComponent)
			END_EXPOSE_GIZMOS(AnimationComponent)
			END_EXPOSE_TO_EDITOR(AnimationComponent)
		};
	}
}