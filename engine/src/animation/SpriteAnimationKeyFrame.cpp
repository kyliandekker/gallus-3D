#include "SpriteAnimationKeyFrame.h"

#include "graphics/dx12/Texture.h"

namespace gallus
{
	namespace animation
	{
		int SpriteAnimationKeyFrame::GetSpriteIndex() const
		{
			return m_iSpriteIndex;
		}

		void SpriteAnimationKeyFrame::SetSpriteIndex(int a_iSpriteIndex)
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
