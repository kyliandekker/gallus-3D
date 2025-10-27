#pragma once

#include "resources/EngineResource.h"

#include <map>

#include "animation/AnimationKeyFrame.h"

#include "utils/FILEPCH.h"

namespace gallus
{
	namespace gameplay
	{
		class EntityID;
	}

	constexpr float FRAME_TIME = 1 / 60.0f;
	namespace animation
	{
		class AnimationTrack : public resources::EngineResource
		{
		public:
			AnimationTrack() = default;

			void Start()
			{ }

			bool LoadByPath(const fs::path& a_Path);

			#ifdef _EDITOR
			bool Save(const fs::path& a_Path) const;

			void AddKeyFrame(int a_iFrame);
			void RemoveKeyFrame(int a_iIndex);
			void Sort();
			#endif

			bool IsValid() const override
			{
				return true;
			}

			bool IsLooping() const
			{
				return m_bIsLooping;
			}

			void SetIsLooping(bool a_bIsLooping)
			{
				m_bIsLooping = a_bIsLooping;
			}

			int GetFrameCount() const
			{
				return m_iFrameCount;
			}

			std::vector<AnimationKeyFrame>& GetKeyFrames();
		private:
			std::vector<AnimationKeyFrame> m_aKeyFrames;
			int m_iFrameCount = 0;
			bool m_bIsLooping = false;
		};
	}
}
