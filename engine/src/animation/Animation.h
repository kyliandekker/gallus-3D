#pragma once

#include "resources/EngineResource.h"

// external
#include <map>
#include <set>

namespace gallus::gameplay
{
	class EntityID;
}
namespace gallus::animation
{
	constexpr float FRAME_TIME = 1 / 60.0f;
	class AnimationKeyFrame;
	enum class AnimationEvent;

	class Animation : public resources::EngineResource
	{
	public:
		/// <summary>
		/// Constructs an animation track.
		/// </summary>
		/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
		Animation();

		~Animation();

		/// <summary>
		/// Loads the animation data.
		/// </summary>
		/// <param name="a_Data">A vector containing data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data) override;

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

		/// <summary>
		/// Adds a key frame to a given frame number.
		/// </summary>
		/// <param name="a_iFrame">The frame number to add the key frame to.</param>
		void AddKeyFrame(size_t a_iFrame);

		/// <summary>
		/// Removes a key frame from a given frame number.
		/// </summary>
		/// <param name="a_iFrame">The frame number to remove the key frame from.</param>
		void RemoveKeyFrame(size_t a_iIndex);

		/// <summary>
		/// Removes a key frame from a given frame number.
		/// </summary>
		/// <param name="a_KeyFrame">The frame number to remove the key frame from.</param>
		void RemoveKeyFrame(AnimationKeyFrame& a_KeyFrame);

		/// <summary>
		/// Sorts the animation track's key frames based on their frame position.
		/// </summary>
		void Sort();

		/// <summary>
		/// Retrieves the index of a key frame at a specific frame, -1 if it does not exist.
		/// </summary>
		/// <returns>The index of the key frame in the array, -1 if it did not exist.</returns>
		int32_t GetKeyFrameAtFrame(size_t a_iIndex);

		/// <summary>
		/// Returns whether the animation loops or not.
		/// </summary>
		/// <returns>True if the animation loops, false otherwise.</returns>
		bool IsLooping() const
		{
			return m_bIsLooping;
		}
		
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
		void SetFrameCount(uint16_t a_iFrameCount)
		{
			m_iFrameCount = a_iFrameCount;
		}

		/// <summary>
		/// Retrieves the amount of frames in the animation track.
		/// </summary>
		/// <returns>The number of frames.</returns>
		uint16_t GetFrameCount() const
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

		uint16_t m_iFrameCount = 0;
		bool m_bIsLooping = false;

		BEGIN_SERIALIZE_PARENT(Animation, resources::EngineResource)
			SERIALIZE_FIELD(m_iFrameCount, "Frame count", "The amount of frames the animation should be (length of the animation).",
				.type = FieldSerializationType::FieldSerializationType_Int16,
				.serializationMethod = SerializationMethod::SerializationMethod_File)
			SERIALIZE_FIELD(m_bIsLooping, "Is Looping", "Whether the animation loops or not.",
				.type = FieldSerializationType::FieldSerializationType_Bool,
				.serializationMethod = SerializationMethod::SerializationMethod_File)
		END_SERIALIZE(Animation)
	};
}