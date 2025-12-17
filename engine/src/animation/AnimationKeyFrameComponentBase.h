#pragma once

// external
#include <string>
#include <rapidjson/document.h>

#include "editor/EditorExpose.h"

namespace gallus
{
	namespace gameplay
	{
		class EntityID;
	}
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
			/// <summary>
			/// Constructs a key frame component.
			/// </summary>
			/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
			AnimationKeyFrameComponentBase(AnimationKeyFrame& a_KeyFrame) : m_KeyFrame(a_KeyFrame)
			{}

			/// <summary>
			/// Activates the key frame component's behaviour.
			/// </summary>
			/// <param name="a_EntityID">The related entity id.</param>
			virtual void Activate(gameplay::EntityID& a_EntityID)
			{}

			/// <summary>
			/// Retrieves the name of the key frame component.
			/// </summary>
			/// <returns>A string containing the name.</returns>
			virtual std::string GetName() const = 0;
#ifdef _EDITOR
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			virtual void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const = 0;
#endif // _EDITOR
			/// <summary>
			/// Retrieves the property name of the key frame component.
			/// </summary>
			/// <returns>A string containing the property name.</returns>
			virtual std::string GetPropertyName() const = 0;

			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			virtual void Deserialize(const resources::SrcData& a_SrcData) = 0;

			AnimationKeyFrame& m_KeyFrame;

			BEGIN_EXPOSE_FIELDS(AnimationKeyFrameComponentBase)
			END_EXPOSE_FIELDS(AnimationKeyFrameComponentBase)
			BEGIN_EXPOSE_GIZMOS(AnimationKeyFrameComponentBase)
			END_EXPOSE_GIZMOS(AnimationKeyFrameComponentBase)
			END_EXPOSE_TO_EDITOR(AnimationKeyFrameComponentBase)
		};
	}
}