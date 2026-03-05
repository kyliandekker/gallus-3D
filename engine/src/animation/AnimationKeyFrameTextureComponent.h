#pragma once

#include "AnimationKeyFrameComponentBase.h"

// external
#include <memory>

#define ANIMATION_KEY_FRAME_TEXTURE_COMPONENT_PROPERTY_NAME "texture"
#define ANIMATION_KEY_FRAME_TEXTURE_COMPONENT_NAME "Texture"

namespace gallus::graphics::dx12
{
	class Texture;
}
namespace gallus::animation
{
	class AnimationKeyFrameTextureComponent : public AnimationKeyFrameComponentBase
	{
	public:
		/// <summary>
		/// Constructs a key frame sprite component.
		/// </summary>
		/// <param name="a_KeyFrame">The key frame this component is attached to.</param>
		AnimationKeyFrameTextureComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
		{}

		/// <summary>
		/// Activates the key frame component's behaviour.
		/// </summary>
		/// <param name="a_EntityID">The related entity id.</param>
		void Activate(gameplay::EntityID& a_EntityID) override;

		/// <summary>
		/// Returns the name of the component.
		/// </summary>
		/// <returns>A string containing the name of the component.</returns>
		std::string GetName() const override;

		/// <summary>
		/// Returns the property name of the component.
		/// </summary>
		/// <returns>A string containing the name of the component used in serialization.</returns>
		const std::string GetPropertyName() const override
		{
			return ANIMATION_KEY_FRAME_TEXTURE_COMPONENT_NAME;
		}
	private:
		std::weak_ptr<graphics::dx12::Texture> m_pTexture = {};
		uint16_t m_iTextureIndex = 0;

		BEGIN_SERIALIZE_PARENT(AnimationKeyFrameTextureComponent, AnimationKeyFrameComponentBase)
		 	SERIALIZE_FIELD(m_pTexture, "Texture", "Pointer to the texture asset used by this sprite. Can be nullptr if no texture is assigned. Determines the visual appearance of the sprite.",
		 		.type = FieldSerializationType::FieldSerializationType_EngineResource,
		 		.assetType = resources::AssetType::Texture,
		 	)
		 	SERIALIZE_FIELD(m_iTextureIndex, "Texture Index", "Index of the sprite within a texture atlas. Used when the texture contains multiple sprites to select which one is displayed.",
		 		.type = FieldSerializationType::FieldSerializationType_Int16
		 	)
		 	SERIALIZE_FIELD(m_pTexture, "Texture Preview", "",
		 		.type = FieldSerializationType::FieldSerializationType_TexturePreview,
		 		.relatedIndexFieldOffset = offsetof(AnimationKeyFrameTextureComponent, m_iTextureIndex)
		 	)
		END_SERIALIZE(AnimationKeyFrameTextureComponent)
	};

	class AnimationKeyFrameTextureSystem : public AnimationKeyFrameBaseSystem
	{
	public:
		std::string GetName() const override;

		const std::string GetPropertyName() const override
		{
			return ANIMATION_KEY_FRAME_TEXTURE_COMPONENT_PROPERTY_NAME;
		}

		AnimationKeyFrameComponentBase* CreateComponent(AnimationKeyFrame& a_KeyFrame) override
		{
			return new AnimationKeyFrameTextureComponent(a_KeyFrame);
		}
	};
}