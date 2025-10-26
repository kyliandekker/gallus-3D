#pragma once

#include "animation/AnimationKeyFrame.h"

namespace gallus
{
	namespace gameplay
	{
		class EntityID;
	}
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
		}
	}
	namespace animation
	{
		class SpriteAnimationKeyFrame : public AnimationKeyFrame
		{
		public:
			SpriteAnimationKeyFrame(int a_iFrame, int a_iSpriteIndex) : AnimationKeyFrame(a_iFrame),
				m_iSpriteIndex(a_iSpriteIndex)
			{}

			void Activate(gameplay::EntityID& a_EntityID) override;

			int GetSpriteIndex() const;

			void SetSpriteIndex(int a_iSpriteIndex);

			graphics::dx12::Texture* GetTexture()
			{
				return m_pTexture;
			}

			const graphics::dx12::Texture* GetTexture() const
			{
				return m_pTexture;
			}

			void SetTexure(graphics::dx12::Texture* a_pTexture)
			{
				m_pTexture = a_pTexture;
			}
		private:
			int m_iSpriteIndex = 0;
			graphics::dx12::Texture* m_pTexture = nullptr;
		};
	}
}
