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
		enum class AnimationEvent;
		class AnimationTrack : public resources::EngineResource
		{
		public:
			/// <summary>
			/// Constructs an animation track.
			/// </summary>
			/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
			AnimationTrack() : EngineResource()
			{
				m_aKeyFrames.reserve(100);
			}

			~AnimationTrack();

			/// <summary>
			/// Loads the animation track by path.
			/// </summary>
			/// <param name="a_Path">The path to load it from</param>
			/// <returns>True if loading was successful, false otherwise.</returns>
			bool LoadByPath(const fs::path& a_Path);

			/// <summary>
			/// Updates the animation track.
			/// </summary>
			/// <param name="a_EntityID">The related entity id.</param>
			/// <param name="a_fDeltaTime">The time that has passed since last frame.</param>
			void Update(gameplay::EntityID& a_EntityID, float a_fDeltaTime);

			/// <summary>
			/// Retrieves the array of key frames that this animation track holds.
			/// </summary>
			/// <returns>An array of key frames in this animation track.</returns>
			std::vector<AnimationKeyFrame*>& GetKeyFrames();
#ifdef _EDITOR
			/// <summary>
			/// Saves animation track by path.
			/// </summary>
			/// <param name="a_Path">The path to load it from.</param>
			/// <returns>True if saving was successful, false otherwise.</returns>
			bool Save(const fs::path& a_Path) const;

			/// <summary>
			/// Adds a key frame to a given frame number.
			/// </summary>
			/// <param name="a_iFrame">The frame number to add the key frame to.</param>
			void AddKeyFrame(int a_iFrame);

			/// <summary>
			/// Removes a key frame from a given frame number.
			/// </summary>
			/// <param name="a_iFrame">The frame number to remove the key frame from.</param>
			void RemoveKeyFrame(int a_iIndex);

			/// <summary>
			/// Removes a key frame from a given frame number.
			/// </summary>
			/// <param name="a_KeyFrame">The frame number to remove the key frame from.</param>
			void RemoveKeyFrame(AnimationKeyFrame& a_KeyFrame);

			/// <summary>
			/// Sorts the animation track's key frames based on their frame position.
			/// </summary>
			void Sort();
#endif
			/// <summary>
			/// Returns whether the resource is a valid resource.
			/// </summary>
			/// <returns>True if the resource was valid, false otherwise.</returns>
			bool IsValid() const override
			{
				return !m_Path.empty() || !m_sName.empty();
			}

			/// <summary>
			/// Returns whether the animation loops or not.
			/// </summary>
			/// <returns>True if the animation loops, false otherwise.</returns>
			bool IsLooping() const
			{
				return m_bIsLooping;
			}
			
#ifdef _EDITOR
			/// <summary>
			/// Sets whether the animation loops or not.
			/// </summary>
			/// <param name="a_bIsLooping">True if the animation should loop, false otherwise.</param>
			void SetIsLooping(bool a_bIsLooping)
			{
				m_bIsLooping = a_bIsLooping;
			}

			/// <summary>
			/// Sets the amount of frames in the animation track.
			/// </summary>
			void SetFrameCount(int a_iFrameCount)
			{
				m_iFrameCount = a_iFrameCount;
			}
#endif
			/// <summary>
			/// Retrieves the amount of frames in the animation track.
			/// </summary>
			/// <returns>The number of frames.</returns>
			int GetFrameCount() const
			{
				return m_iFrameCount;
			}

			/// <summary>
			/// Activates an event.
			/// </summary>
			/// <param name="a_EntityID">The related entity id.</param>
			/// <param name="a_Event">The event that should be activated.</param>
			void ActivateEvent(gameplay::EntityID& a_EntityID, AnimationEvent a_Event);
		private:
			std::vector<AnimationKeyFrame*> m_aKeyFrames;
			std::set<std::string> m_aActiveTags;

			int m_iFrameCount = 0;
			bool m_bIsLooping = false;

#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS(AnimationTrack)
			END_EXPOSE_FIELDS(AnimationTrack)
			BEGIN_EXPOSE_GIZMOS(AnimationTrack)
			END_EXPOSE_GIZMOS(AnimationTrack)
			END_EXPOSE_TO_EDITOR(AnimationTrack)
#endif
		};
	}
}
