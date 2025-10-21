#pragma once

namespace gallus
{
    namespace gameplay
    {
        class EntityID;
    }
	namespace animation
	{
        class AnimationKeyFrame
        {
        public:
            AnimationKeyFrame(int a_iFrame) :
                m_iFrame(a_iFrame)
            { }

            virtual void Activate(const gameplay::EntityID & a_EntityID)
            { }

            int GetFrame() const
            {
                return m_iFrame;
            }

            void SetFrame(int a_iFrame)
            {
                m_iFrame = a_iFrame;
            }
        private:
            int m_iFrame;
        };
	}
}