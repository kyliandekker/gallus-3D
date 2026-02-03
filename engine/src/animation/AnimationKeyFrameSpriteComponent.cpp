#include "animation/AnimationKeyFrameSpriteComponent.h"

#include "core/Engine.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/systems/SpriteSystem.h"

namespace gallus
{
	namespace animation
	{
		//---------------------------------------------------------------------
		void AnimationKeyFrameSpriteComponent::Activate(gameplay::EntityID& a_EntityID)
		{
			gameplay::EntityComponentSystem* ecs = core::ENGINE->GetECS();
			if (!ecs)
			{
				return;
			}

			if (gameplay::SpriteComponent* spriteComp = ecs->GetSystem<gameplay::SpriteSystem>()->TryGetComponent(a_EntityID))
			{
				spriteComp->SetSpriteIndex(m_iSpriteIndex);
				spriteComp->SetTexture(m_pSprite);
			}
		}

		//---------------------------------------------------------------------
		std::string AnimationKeyFrameSpriteComponent::GetName() const
		{
			std::string name = ANIMATION_KEY_FRAME_SPRITE_COMPONENT_NAME;
#ifdef _EDITOR
			name = std::string(font::ICON_IMAGE) + " " + name;
#endif // _EDITOR
			return name;
		}

		//---------------------------------------------------------------------
		std::string AnimationKeyFrameSpriteSystem::GetName() const
		{
			std::string name = ANIMATION_KEY_FRAME_SPRITE_COMPONENT_NAME;
#ifdef _EDITOR
			name = std::string(font::ICON_IMAGE) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}