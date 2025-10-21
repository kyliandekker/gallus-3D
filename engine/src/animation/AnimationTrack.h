#pragma once

#include <vector>

namespace gallus
{
	namespace gameplay
	{
		class EntityID;
	}

	constexpr float FRAME_TIME = 1 / 60.0f;
	namespace animation
	{
		template <typename T>
		class AnimationTrack
		{
		public:
			AnimationTrack() = default;

			void Start()
			{ }

			virtual void Update(const gameplay::EntityID& a_EntityID, float a_fDeltaTime)
			{
				m_fAccumulatedTime += a_fDeltaTime;
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
			float m_fAccumulatedTime = 0.0f;
		};
	}
}
