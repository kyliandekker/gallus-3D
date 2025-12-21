#pragma once

#include "AnimationKeyFrameComponentBase.h"

namespace gallus
{
	namespace audio
	{
		class Sound;
	}
	namespace resources
	{
		class SrcData;
	}
	namespace animation
	{
		class Animation;

		class AnimationKeyFrameSoundComponent : public AnimationKeyFrameComponentBase
		{
		public:
			/// <summary>
			/// Constructs a key frame sprite component.
			/// </summary>
			/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
			AnimationKeyFrameSoundComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
			{
			}

			/// <summary>
			/// Activates the key frame component's behaviour.
			/// </summary>
			/// <param name="a_EntityID">The related entity id.</param>
			void Activate(gameplay::EntityID& a_EntityID) override
			{}

			/// <summary>
			/// Retrieves the name of the key frame component.
			/// </summary>
			/// <returns>A string containing the name.</returns>
			std::string GetName() const override;

			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;

			/// <summary>
			/// Retrieves the property name of the key frame component.
			/// </summary>
			/// <returns>A string containing the property name.</returns>
			std::string GetPropertyName() const override
			{
				return "sound";
			}
#ifdef _EDITOR
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const override;
#endif // _EDITOR
		protected:
			audio::Sound* m_pSound = nullptr;
			
			BEGIN_EXPOSE_FIELDS_PARENT(AnimationKeyFrameSoundComponent, AnimationKeyFrameComponentBase)
				EXPOSE_FIELD(AnimationKeyFrameSoundComponent, m_pSound, "Sound", (FieldOptions{ .type = EditorFieldWidgetType::EditorFieldWidgetType_EngineResource, .assetType = resources::AssetType::Sound }))
			END_EXPOSE_FIELDS(AnimationKeyFrameSoundComponent)
			BEGIN_EXPOSE_GIZMOS(AnimationKeyFrameSoundComponent)
			END_EXPOSE_GIZMOS(AnimationKeyFrameSoundComponent)
			END_EXPOSE_TO_EDITOR(AnimationKeyFrameSoundComponent)
		};
	}
}