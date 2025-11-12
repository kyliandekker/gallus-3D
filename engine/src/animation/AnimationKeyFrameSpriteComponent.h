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
		class AnimationKeyFrameSpriteComponent : public AnimationKeyFrameComponentBase
		{
		public:
			AnimationKeyFrameSpriteComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
			{}

			void Activate(gameplay::EntityID& a_EntityID, AnimationTrack& a_AnimationTrack) override;

			int GetSpriteIndex() const;
			void SetSpriteIndex(int a_iSpriteIndex);

			std::string GetName() const override;

			graphics::dx12::Texture* GetTexture()
			{
				return m_pTexture;
			}

			void SetTexture(graphics::dx12::Texture* a_pTexture)
			{
				m_pTexture = a_pTexture;
			}
#ifdef _EDITOR
			void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const override;

			std::string GetPropertyName() const override
			{
				return "sprite";
			}
#endif
			void Deserialize(const resources::SrcData& a_SrcData) override;
		private:
			graphics::dx12::Texture* m_pTexture = nullptr;
			int m_iSpriteIndex = 0;

			BEGIN_EXPOSED_FIELDS(AnimationKeyFrameSpriteComponent)
				EXPOSE_FIELD(AnimationKeyFrameSpriteComponent, m_pTexture, "Texture", FieldOptions{ .type = EditorWidgetType::AssetPickerPtr, .assetType = resources::AssetType::Sprite })
				EXPOSE_FIELD(AnimationKeyFrameSpriteComponent, m_iSpriteIndex, "Sprite Index", FieldOptions{ .type = EditorWidgetType::DragInt8 })
				EXPOSE_FIELD(AnimationKeyFrameSpriteComponent, m_pTexture, "Texture Preview", FieldOptions{
				.type = EditorWidgetType::TexturePreview,
				.relatedIndexFieldOffset = offsetof(AnimationKeyFrameSpriteComponent, m_iSpriteIndex)
					})
			END_EXPOSED_FIELDS(AnimationKeyFrameSpriteComponent)
		};
	}
}