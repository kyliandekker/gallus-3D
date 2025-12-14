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

#ifdef _EDITOR
// editor
#include "editor/EditorExpose.h"
#endif

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class CommandList;

			class DX12Transform2D;
			class Texture;
			class DX12ShaderBind;
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
			void Init(const gameplay::EntityID& a_EntityID) override;

			/// <summary>
			/// Sets the mesh used by the mesh component.
			/// </summary>
			/// <param name="a_Mesh">Reference to the mesh that the mesh component will use.</param>
			void SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh);

			/// <summary>
			/// Sets the shader used by the mesh component.
			/// </summary>
			/// <param name="a_Shader">Reference to the shader bind that the mesh component will use.</param>
			void SetShader(std::weak_ptr < graphics::dx12::DX12ShaderBind> a_pShaderBind);

			/// <summary>
			/// Sets the texture used by the mesh component.
			/// </summary>
			/// <param name="a_Texture">Reference to the texture that the mesh component will use.</param>
			void SetTexture(std::weak_ptr < graphics::dx12::Texture> a_pTexture);

			/// <summary>
			/// Retrieves the mesh used by the mesh component.
			/// </summary>
			/// <returns>Pointer to the mesh if the mesh exists, otherwise nullptr.</returns>
			std::shared_ptr<graphics::dx12::Mesh> GetMesh() const
			{
				return m_pMesh.lock();
			}

			/// <summary>
			/// Retrieves the shader used by the mesh component.
			/// </summary>
			/// <returns>Pointer to the mesh if the mesh exists, otherwise nullptr.</returns>
			std::shared_ptr<graphics::dx12::DX12ShaderBind> GetShader()
			{
				return m_pShaderBind.lock();
			}

			/// <summary>
			/// Retrieves the texture used by the mesh component.
			/// </summary>
			/// <returns>Pointer to the mesh if the mesh exists, otherwise nullptr.</returns>
			std::shared_ptr<graphics::dx12::Texture > GetTexture()
			{
				return m_pTexture.lock();
			}

			/// <summary>
			/// Retrieves the sprite index.
			/// </summary>
			/// <returns>The sprite index.</returns>
			int8_t GetSpriteIndex()
			{
				return m_iSpriteIndex;
			}

			/// <summary>
			/// Sets the sprite index.
			/// </summary>
			/// <param name="a_iSpriteIndex">The index the sprite should have.</param>
			void SetSpriteIndex(int8_t a_iSpriteIndex);

			/// <summary>
			/// Retrieves the sprite color.
			/// </summary>
			/// <returns>The sprite color.</returns>
			const DirectX::XMFLOAT4& GetColor()
			{
				return m_vColor;
			}

			/// <summary>
			/// Sets the sprite color.
			/// </summary>
			/// <param name="a_vColor">The color the sprite should have.</param>
			void SetColor(const DirectX::XMFLOAT4& a_vColor)
			{
				m_vColor = a_vColor;
			}

			/// <summary>
			/// Renders the mesh.
			/// </summary>
			/// <param name="a_pCommandList">The command list used for rendering.</param>
			/// <param name="a_EntityID">The entity ID to get the data from.</param>
			/// <param name="a_Camera">The camera.</param>
			void Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera);
#ifdef _EDITOR
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;
#endif
			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;
		private:
			std::weak_ptr<graphics::dx12::Mesh> m_pMesh = {};
			std::weak_ptr<graphics::dx12::DX12ShaderBind> m_pShaderBind = {};
			std::weak_ptr<graphics::dx12::Texture> m_pTexture = {};
			int8_t m_iSpriteIndex = 0;
			DirectX::XMFLOAT4 m_vColor = { 1, 1, 1, 1 };
#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS_PARENT(SpriteComponent, Component)
				EXPOSE_FIELD(SpriteComponent, m_pShaderBind, "Shader Bind", (FieldOptions{ .type = EditorFieldWidgetType::ObjectPtr }))
				EXPOSE_FIELD(SpriteComponent, m_pTexture, "Texture", (FieldOptions{ .type = EditorFieldWidgetType::AssetPickerPtr, .assetType = resources::AssetType::Sprite, .description = "Pointer to the texture asset used by this sprite. Can be nullptr if no texture is assigned. Determines the visual appearance of the sprite." }))
				EXPOSE_FIELD(SpriteComponent, m_iSpriteIndex, "Sprite Index", (FieldOptions{ .type = EditorFieldWidgetType::DragInt8, .description = "Index of the sprite within a texture atlas. Used when the texture contains multiple sprites to select which one is displayed." }))
				EXPOSE_FIELD(SpriteComponent, m_pTexture, "Texture Preview", (FieldOptions{
				.type = EditorFieldWidgetType::TexturePreview,
				.relatedIndexFieldOffset = offsetof(SpriteComponent, m_iSpriteIndex)
					}))
			END_EXPOSE_FIELDS(SpriteComponent)
			BEGIN_EXPOSE_GIZMOS(SpriteComponent)
			END_EXPOSE_GIZMOS(SpriteComponent)
			END_EXPOSE_TO_EDITOR(SpriteComponent)
#endif
		};
	}
}