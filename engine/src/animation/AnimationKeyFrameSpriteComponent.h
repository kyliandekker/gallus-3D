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

			void Activate(gameplay::EntityID& a_EntityID) override;

			int GetSpriteIndex() const;
			void SetSpriteIndex(int a_iSpriteIndex);

			std::string GetName() const override
			{
				return "Sprite";
			}

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
				return "spriteComponent";
			}
#endif
			void Deserialize(const resources::SrcData& a_SrcData) override;
		private:
			graphics::dx12::Texture* m_pTexture = nullptr;
			int m_iSpriteIndex = 0;
		};
	}
}