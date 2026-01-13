#pragma once

#include "gameplay/ECSBaseSystem.h"

// external
#include <map>

// gameplay
#include "gameplay/systems/components/AnimationComponent.h"

namespace gallus
{
	namespace animation
	{
		class AnimationKeyFrameBaseSystem;
	}
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// AnimationSystem
		//---------------------------------------------------------------------
		class AnimationSystem : public ECSBaseSystem<AnimationComponent>
		{
		public:
			~AnimationSystem();

			/// <summary>
			/// Initializes the system, setting up necessary resources.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			bool Initialize() override;

			/// <summary>
			/// Retrieves the property name of the system (used in serialization).
			/// </summary>
			/// <returns>A string containing the property name of the system.</returns>
			std::string GetPropertyName() const override;

			/// <summary>
			/// Retrieves the name of the system.
			/// </summary>
			/// <returns>A string containing the name of the system.</returns>
			std::string GetSystemName() const override;

			const std::vector<animation::AnimationKeyFrameBaseSystem*> GetSystems() const
			{
				return m_aSystems;
			}
		protected:
			std::vector<animation::AnimationKeyFrameBaseSystem*> m_aSystems;
		};
	}
}