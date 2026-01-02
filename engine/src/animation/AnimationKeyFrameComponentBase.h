#pragma once

// external
#include <string>

#include "editor/ISerializableObject.h"

namespace gallus
{
	namespace gameplay
	{
		class EntityID;
	}
	namespace animation
	{
		class AnimationKeyFrame;

		class AnimationKeyFrameComponentBase : public ISerializableObject
		{
		public:
			/// <summary>
			/// Constructs a key frame component.
			/// </summary>
			/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
			AnimationKeyFrameComponentBase(AnimationKeyFrame& a_KeyFrame) : m_KeyFrame(a_KeyFrame)
			{
			}

			/// <summary>
			/// Activates the key frame component's behaviour.
			/// </summary>
			/// <param name="a_EntityID">The related entity id.</param>
			virtual void Activate(gameplay::EntityID& a_EntityID)
			{}

			virtual std::string GetName() const = 0;

			virtual const std::string GetPropertyName() const = 0;
		protected:
			AnimationKeyFrame& m_KeyFrame;

			BEGIN_SERIALIZE(AnimationKeyFrameComponentBase)
			END_SERIALIZE(AnimationKeyFrameComponentBase)
		};

		class AnimationKeyFrameBaseSystem
		{
		public:
			virtual std::string GetName() const = 0;
			virtual const std::string GetPropertyName() const = 0;
			virtual AnimationKeyFrameComponentBase* CreateComponent(AnimationKeyFrame& a_KeyFrame) = 0;
		};
	}
}