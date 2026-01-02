#pragma once

#include "AnimationKeyFrameComponentBase.h"

// external
#include <memory>

#define ANIMATION_KEY_FRAME_SPRITE_COMPONENT_NAME "Sprite"
#define ANIMATION_KEY_FRAME_SPRITE_COMPONENT_PROPERTY_NAME "sprite"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
		}
	}
	namespace animation
	{
		class AnimationKeyFrameSpriteComponent : public AnimationKeyFrameComponentBase
		{
		public:
			/// <summary>
			/// Constructs a key frame sprite component.
			/// </summary>
			/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
			AnimationKeyFrameSpriteComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
			{}

			/// <summary>
			/// Activates the key frame component's behaviour.
			/// </summary>
			/// <param name="a_EntityID">The related entity id.</param>
			void Activate(gameplay::EntityID& a_EntityID) override;

			std::string GetName() const override;

			const std::string GetPropertyName() const override
			{
				return ANIMATION_KEY_FRAME_SPRITE_COMPONENT_PROPERTY_NAME;
			}
		private:
			std::weak_ptr<graphics::dx12::Texture> m_pSprite = {};
			int m_iSpriteIndex = 0;

			BEGIN_EXPOSABLE_PARENT(AnimationKeyFrameSpriteComponent, AnimationKeyFrameComponentBase)
			 	EXPOSE_FIELD(m_pSprite, "Sprite", "Pointer to the texture asset used by this sprite. Can be nullptr if no texture is assigned. Determines the visual appearance of the sprite.",
			 		.type = EditorFieldWidgetType::EditorFieldWidgetType_EngineResource,
			 		.assetType = resources::AssetType::Sprite,
			 	)
			 	EXPOSE_FIELD(m_iSpriteIndex, "Sprite Index", "Index of the sprite within a texture atlas. Used when the texture contains multiple sprites to select which one is displayed.",
			 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Int8
			 	)
			 	EXPOSE_FIELD(m_pSprite, "Sprite Preview", "",
			 		.type = EditorFieldWidgetType::EditorFieldWidgetType_TexturePreview,
			 		.relatedIndexFieldOffset = offsetof(AnimationKeyFrameSpriteComponent, m_iSpriteIndex)
			 	)
			END_EXPOSABLE(AnimationKeyFrameSpriteComponent)
		};

		class AnimationKeyFrameSpriteSystem : public AnimationKeyFrameBaseSystem
		{
		public:
			std::string GetName() const override;

			const std::string GetPropertyName() const override
			{
				return ANIMATION_KEY_FRAME_SPRITE_COMPONENT_PROPERTY_NAME;
			}

			AnimationKeyFrameComponentBase* CreateComponent(AnimationKeyFrame& a_KeyFrame) override
			{
				return new AnimationKeyFrameSpriteComponent(a_KeyFrame);
			}
		};
	}
}