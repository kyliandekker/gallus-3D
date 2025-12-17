#pragma once

#include "AnimationKeyFrameComponentBase.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
		}
	}
	namespace resources
	{
		class SrcData;
	}
	namespace animation
	{
		class AnimationTrack;

		class AnimationKeyFrameTagComponent : public AnimationKeyFrameComponentBase
		{
		public:
			/// <summary>
			/// Constructs a key frame sprite component.
			/// </summary>
			/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
			AnimationKeyFrameTagComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
			{
			}

			/// <summary>
			/// Activates the key frame component's behaviour.
			/// </summary>
			/// <param name="a_EntityID">The related entity id.</param>
			void Activate(gameplay::EntityID& a_EntityID) override;

			/// <summary>
			/// Retrieves the name of the key frame component.
			/// </summary>
			/// <returns>A string containing the name.</returns>
			std::string GetName() const override;
#ifdef _EDITOR
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const override;

			/// <summary>
			/// Retrieves the tag.
			/// </summary>
			/// <returns>The tag.</returns>
			const std::string& GetTag() const
			{
				return m_sTag;
			}

			/// <summary>
			/// Sets the tag used in this key frame.
			/// </summary>
			/// <param name="a_sTag">The tag.</param>
			void SetTag(const std::string& a_sTag)
			{
				m_sTag = a_sTag;
			}
#endif // _EDITOR
			/// <summary>
			/// Retrieves the property name of the key frame component.
			/// </summary>
			/// <returns>A string containing the property name.</returns>
			std::string GetPropertyName() const override
			{
				return "tag";
			}

			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;
		private:
			std::string m_sTag;

			BEGIN_EXPOSE_FIELDS_PARENT(AnimationKeyFrameTagComponent, AnimationKeyFrameComponentBase)
			END_EXPOSE_FIELDS(AnimationKeyFrameTagComponent)
			BEGIN_EXPOSE_GIZMOS(AnimationKeyFrameTagComponent)
			END_EXPOSE_GIZMOS(AnimationKeyFrameTagComponent)
			END_EXPOSE_TO_EDITOR(AnimationKeyFrameTagComponent)
		};
	}
}