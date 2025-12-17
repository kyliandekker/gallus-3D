#include "animation/AnimationKeyFrameSpriteComponent.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/CommandQueue.h"

#include "graphics/imgui/font_icon.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/systems/SpriteSystem.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace animation
	{
		//---------------------------------------------------------------------
		void AnimationKeyFrameSpriteComponent::Activate(gameplay::EntityID& a_EntityID)
		{
			gameplay::SpriteComponent& spriteComp = core::ENGINE->GetECS().GetSystem<gameplay::SpriteSystem>().GetComponent(a_EntityID);
			spriteComp.SetSpriteIndex(m_iSpriteIndex);
			spriteComp.SetTexture(m_pTexture);
		}

#ifdef _EDITOR
		//---------------------------------------------------------------------
		int AnimationKeyFrameSpriteComponent::GetSpriteIndex() const
		{
			return m_iSpriteIndex;
		}

		//---------------------------------------------------------------------
		void AnimationKeyFrameSpriteComponent::SetSpriteIndex(int a_iSpriteIndex)
		{
			if (auto tex = m_pTexture.lock())
			{
				if (a_iSpriteIndex >= tex->GetSpriteRectsSize())
				{
					a_iSpriteIndex = tex->GetSpriteRectsSize() - 1;
				}
			}
			if (a_iSpriteIndex < 0)
			{
				a_iSpriteIndex = 0;
			}
			m_iSpriteIndex = a_iSpriteIndex;
		}
#endif // _EDITOR

		//---------------------------------------------------------------------
		std::string AnimationKeyFrameSpriteComponent::GetName() const
		{
			std::string name = "Sprite";
#ifdef _EDITOR
			name = std::string(font::ICON_IMAGE) + " " + name;
#endif // _EDITOR
			return name;
		}

#ifdef _EDITOR
		//---------------------------------------------------------------------
		void AnimationKeyFrameSpriteComponent::Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator)  const
		{
			a_Value.AddMember("spriteIndex", m_iSpriteIndex, a_Allocator);
			if (auto tex = m_pTexture.lock())
			{
				std::string texName = tex->GetName();
				a_Value.AddMember(
					"texture",
					rapidjson::Value(texName.c_str(), a_Allocator),
					a_Allocator
				);
			}
		}
#endif _EDITOR

		//---------------------------------------------------------------------
		void AnimationKeyFrameSpriteComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			std::string tex = "";
			if (!a_SrcData.GetString("texture", tex))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Animation sprite component did not have key %s present in its meta data.", "texture");
			}

			if (!a_SrcData.GetInt("spriteIndex", m_iSpriteIndex))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Animation sprite component did not have key %s present in its meta data.", "spriteIndex");
			}

			std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = core::ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
			if (!tex.empty())
			{
				m_pTexture = core::ENGINE->GetResourceAtlas().LoadTexture(tex, cCommandQueue);
			}
			cCommandQueue->Flush();
		}
	}
}