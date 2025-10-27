#pragma once

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
		}
	}
	namespace gameplay
	{
		class EntityID;
	}
	namespace animation
	{
		class AnimationKeyFrameComponentBase
		{
		public:
			virtual void Activate(gameplay::EntityID& a_EntityID)
			{
			}

			bool m_bUsed = false;
		};

		class AnimationKeyFrameSpriteComponent : public AnimationKeyFrameComponentBase
		{
		public:
			void Activate(gameplay::EntityID& a_EntityID) override;

			int GetSpriteIndex() const;
			void SetSpriteIndex(int a_iSpriteIndex);
		private:
			graphics::dx12::Texture* m_pTexture = nullptr;
			int m_iSpriteIndex = 0;
		};

		class AnimationKeyFrame
		{
		public:
			AnimationKeyFrame() = default;

			AnimationKeyFrame(int a_iFrame) :
				m_iFrame(a_iFrame)
			{ }

			virtual void Activate(gameplay::EntityID & a_EntityID)
			{ }

			int GetFrame() const
			{
				return m_iFrame;
			}

			void SetFrame(int a_iFrame)
			{
				m_iFrame = a_iFrame;
			}

			// Comparison operators
			bool operator<(const AnimationKeyFrame& a_Other) const
			{
				return m_iFrame < a_Other.m_iFrame;
			}

			bool operator>(const AnimationKeyFrame& a_Other) const
			{
				return m_iFrame > a_Other.m_iFrame;
			}

			AnimationKeyFrame(const AnimationKeyFrame&) = default;
			AnimationKeyFrame(AnimationKeyFrame&&) noexcept = default;
			AnimationKeyFrame& operator=(const AnimationKeyFrame&) = default;
			AnimationKeyFrame& operator=(AnimationKeyFrame&&) noexcept = default;
		private:
			int m_iFrame = 0;

			AnimationKeyFrameSpriteComponent m_AnimationKeyFrameSpriteComponent;
		};
	}
}
