#pragma once

// external
#include <vector>
#ifdef _EDITOR
#include <rapidjson/document.h>
#endif// _EDITOR

// animation
#include "animation/AnimationKeyFrameComponentBase.h"

// gameplay
#include "gameplay/EntityID.h"

namespace gallus
{
	namespace resources
	{
		class SrcData;
	}
	namespace animation
	{
		class AnimationTrack;

		class AnimationKeyFrame
		{
		public:
			~AnimationKeyFrame();

			/// <summary>
			/// Constructs a key frame at a given frame.
			/// </summary>
			/// <param name="a_iFrame">The frame where the key frame sits.</param>
			AnimationKeyFrame(int a_iFrame, AnimationTrack& a_AnimationTrack) :
				m_iFrame(a_iFrame),
				m_pAnimationTrack(&a_AnimationTrack)
			{ }

			/// <summary>
			/// Converts an animation event enumeration value to its corresponding string representation.
			/// </summary>
			/// <param name="a_EntityID">The entity id that holds the animation component.</param>
			void Activate(gameplay::EntityID& a_EntityID);

			/// <summary>
			/// Retrieves the frame where the key frame activates.
			/// </summary>
			/// <returns>A number representing the frame.</returns>
			int GetFrame() const
			{
				return m_iFrame;
			}

#ifdef _EDITOR
			/// <summary>
			/// Sets the frame where this key frame activates.
			/// </summary>
			/// <param name="a_iFrame">The frame.</param>
			void SetFrame(int a_iFrame)
			{
				m_iFrame = a_iFrame;
			}
#endif // _EDITOR
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
			template <typename T>
			T* AddComponent()
			{
				if (HasComponent<T>())
				{
					return GetComponent<T>();
				}

				T* comp = new T(*this);
				m_aAnimationKeyFrameComponents.push_back(comp);
				return comp;
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

			AnimationTrack* GetAnimationTrack()
			{
				return m_pAnimationTrack;
			}
#ifdef _EDITOR
			/// <summary>
			/// Retrieves the array of components that this key frame holds.
			/// </summary>
			/// <returns>An array of components attached to this key frame.</returns>
			std::vector<AnimationKeyFrameComponentBase*>& GetComponents()
			{
				return m_aAnimationKeyFrameComponents;
			}

			/// <summary>
			/// Serialized the key frame to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const;

			/// <summary>
			/// Removes a component from the key frame.
			/// </summary>
			/// <param name="a_pAnimationKeyFrameComponent">The component to remove.</param>
			void RemoveComponent(AnimationKeyFrameComponentBase* a_pAnimationKeyFrameComponent)
			{
				size_t i = 0;
				for (AnimationKeyFrameComponentBase* keyFrame : m_aAnimationKeyFrameComponents)
				{
					if (keyFrame == a_pAnimationKeyFrameComponent)
					{
						m_aAnimationKeyFrameComponents.erase(m_aAnimationKeyFrameComponents.begin() + i);
						break;
					}
				}
			}
#endif // _EDITOR
			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData);
		private:
			int m_iFrame = 0;

			std::vector<AnimationKeyFrameComponentBase*> m_aAnimationKeyFrameComponents;
			AnimationTrack* m_pAnimationTrack = nullptr;
		};
	}
}
