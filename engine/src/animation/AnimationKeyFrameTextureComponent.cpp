#include "animation/AnimationKeyFrameTextureComponent.h"

// core
#include "core/Engine.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/systems/SpriteSystem.h"

namespace gallus::animation
{
	//---------------------------------------------------------------------
	void AnimationKeyFrameTextureComponent::Activate(gameplay::EntityID& a_EntityID)
	{
		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		if (gameplay::SpriteComponent* spriteComp = ecs->GetSystem<gameplay::SpriteSystem>()->TryGetComponent(a_EntityID))
		{
			spriteComp->SetTextureIndex(m_iTextureIndex);
			spriteComp->SetTexture(m_pTexture);
		}
	}

	//---------------------------------------------------------------------
	std::string AnimationKeyFrameTextureComponent::GetName() const
	{
		std::string name = ANIMATION_KEY_FRAME_TEXTURE_COMPONENT_NAME;
		return name;
	}

	//---------------------------------------------------------------------
	std::string AnimationKeyFrameTextureSystem::GetName() const
	{
		std::string name = ANIMATION_KEY_FRAME_TEXTURE_COMPONENT_NAME;
		return name;
	}
}