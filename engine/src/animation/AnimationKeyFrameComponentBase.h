#pragma once

#include "gameplay/EntityID.h"

#include "editor/EditorExpose.h"

#include <string>
#ifdef _EDITOR
#include <rapidjson/document.h>
#endif

namespace gallus
{
	namespace resources
	{
		class SrcData;
	}
	namespace animation
	{
		class AnimationKeyFrame;
		class AnimationTrack;

		class AnimationKeyFrameComponentBase : public IExposableToEditor
		{
		public:
			AnimationKeyFrameComponentBase(AnimationKeyFrame& a_KeyFrame) : m_KeyFrame(a_KeyFrame)
			{}

			virtual void Activate(gameplay::EntityID& a_EntityID, AnimationTrack& a_AnimationTrack)
			{}

			virtual std::string GetName() const = 0;

			bool m_bEnabled = false;

#ifdef _EDITOR
			virtual void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const = 0;
#endif
			virtual std::string GetPropertyName() const = 0;

			virtual void Deserialize(const resources::SrcData& a_SrcData) = 0;

			AnimationKeyFrame& m_KeyFrame;

			BEGIN_EXPOSED_FIELDS(AnimationKeyFrameComponentBase)
			END_EXPOSED_FIELDS(AnimationKeyFrameComponentBase)
		};
	}
}