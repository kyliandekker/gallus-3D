#pragma once

#include "resources/ISerializableObject.h"

// external
#include <string>

namespace gallus::gameplay
{
	struct EntityID;
}
namespace gallus::animation
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
		virtual void Activate(gameplay::EntityID& a_EntityID);

		/// <summary>
		/// Returns the name of the component.
		/// </summary>
		/// <returns>A string containing the name of the component.</returns>
		virtual std::string GetName() const = 0;

		/// <summary>
		/// Returns the property name of the component.
		/// </summary>
		/// <returns>A string containing the name of the component used in serialization.</returns>
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