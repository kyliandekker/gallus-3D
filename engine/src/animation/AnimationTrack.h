#pragma once

#include <vector>

#include "animation/AnimationKeyFrame.h"

namespace gallus
{
	namespace gameplay
	{
		class EntityID;
	}

	constexpr float FRAME_TIME = 1 / 60.0f;
	namespace animation
	{
		class AnimationTrackBase
		{
		public:
			virtual void Start()
			{ }

			virtual void Update(gameplay::EntityID& a_EntityID, float a_fDeltaTime)
			{
				m_fAccumulatedTime += a_fDeltaTime;
			}

			virtual AnimationKeyFrame* GetKeyFrame(size_t a_iIndex) = 0;
			virtual const AnimationKeyFrame* GetKeyFrame(size_t a_iIndex) const = 0;
			virtual size_t GetKeyFramesSize() const = 0;
		protected:
			float m_fAccumulatedTime = 0.0f;
		};

		template <typename T>
		class AnimationTrack : public AnimationTrackBase
		{
		public:
			AnimationTrack() = default;

			void Start() override
			{ }

			void Update(gameplay::EntityID& a_EntityID, float a_fDeltaTime) override
			{
				AnimationTrackBase::Update(a_EntityID, a_fDeltaTime);
				for (size_t i = 0; i < m_aKeyFrames.size(); i++)
				{
					if (m_fAccumulatedTime >= FRAME_TIME * i)
					{
						m_aKeyFrames[i].Activate(a_EntityID);
					}
					else
					{
						break;
					}
				}
			}

			const AnimationKeyFrame* GetKeyFrame(size_t a_iIndex) const override
			{
				return &m_aKeyFrames[a_iIndex];
			}

			AnimationKeyFrame* GetKeyFrame(size_t a_iIndex) override
			{
				return &m_aKeyFrames[a_iIndex];
			}

			size_t GetKeyFramesSize() const override
			{
				return m_aKeyFrames.size();
			}

			const std::vector<T>& GetKeyFrames() const
			{
				return m_aKeyFrames;
			}

			std::vector<T>& GetKeyFrames()
			{
				return m_aKeyFrames;
			}
private:
			std::vector<T> m_aKeyFrames;
		};
	}
}
