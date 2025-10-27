#include "animation/AnimationKeyFrame.h"

#include "graphics/dx12/Texture.h"

#include "gameplay/systems/SpriteSystem.h"

namespace gallus
{
	namespace animation
	{
		void AnimationKeyFrameSpriteComponent::Activate(gameplay::EntityID& a_EntityID)
		{
			gameplay::SpriteComponent& spriteComp = core::ENGINE->GetECS().GetSystem<gameplay::SpriteSystem>().GetComponent(a_EntityID);
			spriteComp.SetSpriteIndex(m_iSpriteIndex);
			if (m_pTexture)
			{
				spriteComp.SetTexture(m_pTexture);
			}
		}

		int AnimationKeyFrameSpriteComponent::GetSpriteIndex() const
		{
			return m_iSpriteIndex;
		}

		void AnimationKeyFrameSpriteComponent::SetSpriteIndex(int a_iSpriteIndex)
		{
			if (m_pTexture && a_iSpriteIndex >= m_pTexture->GetSpriteRectsSize())
			{
				a_iSpriteIndex = m_pTexture->GetSpriteRectsSize() - 1;
			}
			if (a_iSpriteIndex < 0)
			{
				a_iSpriteIndex = 0;
			}
			m_iSpriteIndex = a_iSpriteIndex;
		}
	}
}