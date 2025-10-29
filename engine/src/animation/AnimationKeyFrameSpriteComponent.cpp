#include "animation/AnimationKeyFrameSpriteComponent.h"

#include "graphics/dx12/Texture.h"
#include "graphics/dx12/CommandQueue.h"

#include "gameplay/systems/SpriteSystem.h"

#include "resources/SrcData.h"

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

#ifdef _EDITOR
		void AnimationKeyFrameSpriteComponent::Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator)  const
		{
			a_Value.AddMember("spriteIndex", m_iSpriteIndex, a_Allocator);
			std::string tex = m_pTexture->GetName();
			a_Value.AddMember(
				"texture",
				rapidjson::Value(tex.c_str(), a_Allocator),
				a_Allocator
			);
		}
#endif _EDITOR
		void AnimationKeyFrameSpriteComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			std::string tex = a_SrcData.GetString("texture");
			m_iSpriteIndex = a_SrcData.GetInt("spriteIndex");

			std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = core::ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
			if (!tex.empty())
			{
				SetTexture(core::ENGINE->GetResourceAtlas().LoadTexture(tex, cCommandQueue).get());
			}
			cCommandQueue->Flush();
		}
	}
}