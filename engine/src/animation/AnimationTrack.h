#pragma once

#include "resources/EngineResource.h"

#include <map>
#include <set>

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
			~AnimationTrack();

			void Start()
			{ }

			bool LoadByPath(const fs::path& a_Path);

			void Update(gameplay::EntityID& a_EntityID, float a_fDeltaTime);

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

			void ActivateTag(const std::string& a_sTag)
			{
				m_aActiveTags.insert(a_sTag);
			}

			void DeactivateTag(const std::string& a_sTag)
			{
				m_aActiveTags.erase(a_sTag);
			}

			std::vector<AnimationKeyFrame*>& GetKeyFrames();
		private:
			std::vector<AnimationKeyFrame*> m_aKeyFrames;
			std::set<std::string> m_aActiveTags;

			int m_iFrameCount = 0;
			int m_iNextKeyFrameIndex = 0;
			bool m_bIsLooping = false;

			float m_fAccumulatedTime = 0.0f;
		};
	}
}
