#pragma once

// external
#ifdef _EDITOR
#include <string>
#endif // _EDITOR

namespace gallus
{
	namespace animation
	{
		enum class AnimationEvent
		{
			AnimationEvent_None,
			AnimationEvent_Delete
		};

#ifdef _EDITOR
		/// <summary>
		/// Converts an animation event enumeration value to its corresponding string representation.
		/// </summary>
		/// <param name="a_AnimationEvent">The animation event to convert.</param>
		/// <returns>A string representing the specified animation event.</returns>
		inline std::string AnimationEventToString(AnimationEvent a_AnimationEvent)
		{
			switch (a_AnimationEvent)
			{
				case AnimationEvent::AnimationEvent_None:
				{
					return "None";
				}
				case AnimationEvent::AnimationEvent_Delete:
				{
					return "Delete";
				}
				default:
				{
					return "";
				}
			}
		}
#endif // _EDITOR
	}
}