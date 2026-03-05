#pragma once

// external
#include <vector>

// animation
#include "animation/AnimationKeyFrameComponentBase.h"

namespace gallus::gameplay
{
	struct EntityID;
}
namespace gallus::resources
{
	class SrcData;
}
namespace gallus::animation
{
	class Animation;

	class AnimationKeyFrame
	{
	public:
		~AnimationKeyFrame();

		/// <summary>
		/// Constructs a key frame at a given frame.
		/// </summary>
		/// <param name="a_iFrame">The frame where the key frame sits.</param>
		AnimationKeyFrame(size_t a_iFrame, Animation& a_Animation) :
			m_iFrame(a_iFrame),
			m_pAnimation(a_Animation)
		{}

		/// <summary>
		/// Converts an animation event enumeration value to its corresponding string representation.
		/// </summary>
		/// <param name="a_EntityID">The entity id that holds the animation component.</param>
		void Activate(gameplay::EntityID& a_EntityID);

		/// <summary>
		/// Retrieves the frame where the key frame activates.
		/// </summary>
		/// <returns>A number representing the frame.</returns>
		size_t GetFrame() const
		{
			return m_iFrame;
		}

#ifndef _RELEASE
		/// <summary>
		/// Sets the frame where this key frame activates.
		/// </summary>
		/// <param name="a_iFrame">The frame.</param>
		void SetFrame(size_t a_iFrame)
		{
			m_iFrame = a_iFrame;
		}
#endif // _RELEASE

		bool operator<(const AnimationKeyFrame& a_Other) const
		{
			return m_iFrame < a_Other.m_iFrame;
		}

		bool operator>(const AnimationKeyFrame& a_Other) const
		{
			return m_iFrame > a_Other.m_iFrame;
		}

		/// <summary>
		/// Checks whether a key frame has a component.
		/// </summary>
		/// <returns>True if the key frame contains this component, false otherwise.</returns>
		template <typename T>
		bool HasComponent()
		{
			for (AnimationKeyFrameComponentBase* component : m_aAnimationKeyFrameComponents)
			{
				if (dynamic_cast<T*>(component))
				{
					return true;
				}
			}
			return false;
		};

		/// <summary>
		/// Adds a component (if it does not already exist) and returns it.
		/// </summary>
		/// <returns>The component that was added.</returns>
		void AddComponent(AnimationKeyFrameComponentBase* a_pComponent)
		{
			m_aAnimationKeyFrameComponents.push_back(a_pComponent);
		};

		/// <summary>
		/// Retrieves a component (if it exists).
		/// </summary>
		/// <returns>The component that was requested.</returns>
		template <typename T>
		T* GetComponent()
		{
			for (AnimationKeyFrameComponentBase* component : m_aAnimationKeyFrameComponents)
			{
				T* castedComp = dynamic_cast<T*>(component);
				if (castedComp)
				{
					return castedComp;
				}
			}
			return nullptr;
		};

		/// <summary>
		/// Retrieves the animation that owns this key frame.
		/// </summary>
		/// <returns>A pointer to the animation.</returns>
		Animation& GetAnimation()
		{
			return m_pAnimation;
		}
#ifndef _RELEASE
		/// <summary>
		/// Retrieves the array of components that this key frame holds.
		/// </summary>
		/// <returns>An array of components attached to this key frame.</returns>
		std::vector<AnimationKeyFrameComponentBase*>& GetComponents()
		{
			return m_aAnimationKeyFrameComponents;
		}

		/// <summary>
		/// Removes a component from the key frame.
		/// </summary>
		/// <param name="a_pAnimationKeyFrameComponent">The component to remove.</param>
		void RemoveComponent(AnimationKeyFrameComponentBase* a_pAnimationKeyFrameComponent);
#endif // _RELEASE
	private:
		size_t m_iFrame = 0;

		std::vector<AnimationKeyFrameComponentBase*> m_aAnimationKeyFrameComponents;
		Animation& m_pAnimation;
	};
}