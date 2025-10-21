#pragma once

#include "animation/AnimationKeyFrame.h"

namespace gallus
{
    namespace gameplay
    {
        class EntityID;
    }
    namespace animation
    {
        class SpriteAnimationKeyFrame : public AnimationKeyFrame
        {
        public:
            SpriteAnimationKeyFrame(int a_iFrame, int a_iSpriteIndex) : AnimationKeyFrame(a_iFrame),
                m_iSpriteIndex(a_iSpriteIndex)
            {}

            void Activate(const gameplay::EntityID& a_EntityID) override
            { }

            int GetSpriteIndex() const
            {
                return m_iSpriteIndex;
            }

            void SetSpriteIndex(int a_iSpriteIndex)
            {
                m_iSpriteIndex = a_iSpriteIndex;
            }
        private:
            int m_iSpriteIndex = 0;
        };
    }
}