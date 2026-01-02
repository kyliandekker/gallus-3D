#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <memory>
#include <glm/vec4.hpp>

// graphics
#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/Camera.h"

// resources
#include "resources/AssetType.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class CommandList;

			class Transform;
			class Texture;
			class ShaderBind;
			class Mesh;
			class Camera;
		}
	}
	namespace gameplay
	{
		struct EntityID;

		//---------------------------------------------------------------------
		// SpriteComponent
		//---------------------------------------------------------------------
		class SpriteComponent : public Component
		{
		public:
			/// <summary>
			/// Initializes the component.
			/// </summary>
			void SetDefaults(const gameplay::EntityID& a_EntityID) override;

			/// <summary>
			/// Sets the mesh used by the mesh component.
			/// </summary>
			/// <param name="a_Mesh">Reference to the mesh that the mesh component will use.</param>
			void SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh);

			/// <summary>
			/// Sets the shader used by the mesh component.
			/// </summary>
			/// <param name="a_Shader">Reference to the shader bind that the mesh component will use.</param>
			void SetShader(std::weak_ptr < graphics::dx12::ShaderBind> a_pShaderBind);

			/// <summary>
			/// Sets the texture used by the mesh component.
			/// </summary>
			/// <param name="a_Texture">Reference to the texture that the mesh component will use.</param>
			void SetTexture(std::weak_ptr < graphics::dx12::Texture> a_pTexture);

			/// <summary>
			/// Sets the sprite index.
			/// </summary>
			/// <param name="a_iSpriteIndex">The index the sprite should have.</param>
			void SetSpriteIndex(int8_t a_iSpriteIndex);
			
			/// <summary>
			/// Retrieves the layer order.
			/// </summary>
			/// <returns>An integer representing the layer order.</returns>
			int8_t GetOrder() const
			{
				return m_iOrder;
			}

			/// <summary>
			/// Renders the mesh.
			/// </summary>
			/// <param name="a_pCommandList">The command list used for rendering.</param>
			/// <param name="a_EntityID">The entity ID to get the data from.</param>
			/// <param name="a_Camera">The camera.</param>
			void Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera);
		private:
			void OnOrderChanged();

			bool m_bIsStatic = false;
			int8_t m_iOrder = false;

			std::weak_ptr<graphics::dx12::Mesh> m_pMesh = {};
			std::weak_ptr<graphics::dx12::ShaderBind> m_pShaderBind = {};
			std::weak_ptr<graphics::dx12::Texture> m_pSprite = {};
			int8_t m_iSpriteIndex = 0;
			DirectX::XMFLOAT4 m_vColor = { 1, 1, 1, 1 };

				BEGIN_EXPOSABLE_PARENT(SpriteComponent, Component)
				 	EXPOSE_FIELD(m_bIsStatic, "Static", "Determines whether the sprite should stick on the screen or not.",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Bool)
				 	EXPOSE_FIELD(m_iOrder, "Order", "Determines what sprites overlap other sprites.",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Int8,
				 		.onChangeFunc = MakeOnChangeFunc(&SpriteComponent::OnOrderChanged))
				 	EXPOSE_FIELD(m_vColor, "Color", "Determines what color the sprite has.",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Color)
				 	EXPOSE_FIELD(m_pShaderBind, "Shader Bind", "",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_ObjectPtr)
				 	EXPOSE_FIELD(m_pSprite, "Sprite", "Pointer to the texture asset used by this sprite. Can be nullptr if no texture is assigned. Determines the visual appearance of the sprite.",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_EngineResource,
				 		.assetType = resources::AssetType::Sprite)
				 	EXPOSE_FIELD(m_iSpriteIndex, "Sprite Index", "Index of the sprite within a texture atlas. Used when the texture contains multiple sprites to select which one is displayed.",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Int8)
				 	EXPOSE_FIELD(m_pSprite, "Sprite Preview", "",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_TexturePreview,
				 		.relatedIndexFieldOffset = offsetof(SpriteComponent, m_iSpriteIndex))
				END_EXPOSABLE(SpriteComponent)
		};
	}
}