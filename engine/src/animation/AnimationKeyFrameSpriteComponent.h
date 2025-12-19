#pragma once

#include "AnimationKeyFrameComponentBase.h"

// external
#include <memory>

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
		class AnimationKeyFrameSpriteComponent : public AnimationKeyFrameComponentBase
		{
		public:
			/// <summary>
			/// Constructs a key frame sprite component.
			/// </summary>
			/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
			AnimationKeyFrameSpriteComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
			{}

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
			/// Retrieves the sprite index.
			/// </summary>
			/// <returns>The sprite index.</returns>
			int GetSpriteIndex() const;

			/// <summary>
			/// Retrieves the texture.
			/// </summary>
			/// <returns>A pointer to the texture.</returns>
			std::weak_ptr<graphics::dx12::Texture> GetTexture()
			{
				return m_pTexture;
			}

			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const override;

			/// <summary>
			/// Sets the texture used in this key frame.
			/// </summary>
			/// <param name="a_pTexture">The texture.</param>
			void SetTexture(std::weak_ptr<graphics::dx12::Texture> a_pTexture)
			{
				m_pTexture = a_pTexture;
			}

			/// <summary>
			/// Sets the sprite index of the texture used in this key frame.
			/// </summary>
			/// <param name="a_iSpriteIndex">The index of the sprite rect.</param>
			void SetSpriteIndex(int a_iSpriteIndex);
#endif // _EDITOR
			/// <summary>
			/// Retrieves the property name of the key frame component.
			/// </summary>
			/// <returns>A string containing the property name.</returns>
			std::string GetPropertyName() const override
			{
				return "sprite";
			}

			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;
		private:
			std::weak_ptr<graphics::dx12::Texture> m_pTexture = {};
			int m_iSpriteIndex = 0;

			BEGIN_EXPOSE_FIELDS_PARENT(AnimationKeyFrameSpriteComponent, AnimationKeyFrameComponentBase)
				EXPOSE_FIELD(AnimationKeyFrameSpriteComponent, m_pTexture, "Texture", (FieldOptions{ .type = EditorFieldWidgetType::EditorFieldWidgetType_EngineResource, .assetType = resources::AssetType::Sprite }))
				EXPOSE_FIELD(AnimationKeyFrameSpriteComponent, m_iSpriteIndex, "Sprite Index", (FieldOptions{ .type = EditorFieldWidgetType::EditorFieldWidgetType_Int8 }))
				EXPOSE_FIELD(AnimationKeyFrameSpriteComponent, m_pTexture, "Texture Preview", (FieldOptions{
				.type = EditorFieldWidgetType::EditorFieldWidgetType_TexturePreview,
				.relatedIndexFieldOffset = offsetof(AnimationKeyFrameSpriteComponent, m_iSpriteIndex)
					}))
			END_EXPOSE_FIELDS(AnimationKeyFrameSpriteComponent)
			BEGIN_EXPOSE_GIZMOS(AnimationKeyFrameSpriteComponent)
			END_EXPOSE_GIZMOS(AnimationKeyFrameSpriteComponent)
			END_EXPOSE_TO_EDITOR(AnimationKeyFrameSpriteComponent)
		};
	}
}