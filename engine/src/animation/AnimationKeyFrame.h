#pragma once

#include <vector>
#ifdef _EDITOR
#include <rapidjson/document.h>
#endif

#include "gameplay/EntityID.h"

#include "animation/AnimationKeyFrameComponentBase.h"

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
			AnimationKeyFrame() = default;

			AnimationKeyFrame(int a_iFrame) :
				m_iFrame(a_iFrame)
			{ }

			void Activate(gameplay::EntityID& a_EntityID, AnimationTrack& a_AnimationTrack);

			int GetFrame() const
			{
				return m_iFrame;
			}

			void SetFrame(int a_iFrame)
			{
				m_iFrame = a_iFrame;
			}

			bool operator<(const AnimationKeyFrame& a_Other) const
			{
				return m_iFrame < a_Other.m_iFrame;
			}

			bool operator>(const AnimationKeyFrame& a_Other) const
			{
				return m_iFrame > a_Other.m_iFrame;
			}

			AnimationKeyFrame(const AnimationKeyFrame&) = default;
			AnimationKeyFrame(AnimationKeyFrame&&) noexcept = default;
			AnimationKeyFrame& operator=(const AnimationKeyFrame&) = default;
			AnimationKeyFrame& operator=(AnimationKeyFrame&&) noexcept = default;

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

#ifdef _EDITOR
			void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const;
#endif
			void Deserialize(const resources::SrcData& a_SrcData);
		private:
			int m_iFrame = 0;

			std::vector<AnimationKeyFrameComponentBase*> m_aAnimationKeyFrameComponents;
		};
	}
}
