#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <memory>
#include <glm/vec4.hpp>

// gameplay includes
#include "gameplay/systems/components/Component.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class CommandList;

			class DX12Transform;
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
			void SetMesh(graphics::dx12::Mesh* a_pMesh);

			/// <summary>
			/// Sets the shader used by the mesh component.
			/// </summary>
			/// <param name="a_Shader">Reference to the shader bind that the mesh component will use.</param>
			void SetShader(graphics::dx12::DX12ShaderBind* a_pShaderBind);

			/// <summary>
			/// Sets the texture used by the mesh component.
			/// </summary>
			/// <param name="a_Texture">Reference to the texture that the mesh component will use.</param>
			void SetTexture(graphics::dx12::Texture* a_pTexture);

			/// <summary>
			/// Retrieves the mesh used by the mesh component.
			/// </summary>
			/// <returns>Pointer to the mesh if the mesh exists, otherwise nullptr.</returns>
			graphics::dx12::Mesh* GetMesh() const
			{
				return m_pMesh;
			}

			/// <summary>
			/// Retrieves the shader used by the mesh component.
			/// </summary>
			/// <returns>Pointer to the mesh if the mesh exists, otherwise nullptr.</returns>
			graphics::dx12::DX12ShaderBind* GetShader()
			{
				return m_pShaderBind;
			}

			/// <summary>
			/// Retrieves the texture used by the mesh component.
			/// </summary>
			/// <returns>Pointer to the mesh if the mesh exists, otherwise nullptr.</returns>
			graphics::dx12::Texture* GetTexture()
			{
				return m_pTexture;
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

			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;

			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;
		private:
			graphics::dx12::Mesh* m_pMesh = nullptr;
			graphics::dx12::DX12ShaderBind* m_pShaderBind = nullptr;
			graphics::dx12::Texture* m_pTexture = nullptr;
			int8_t m_iSpriteIndex = 0;
			DirectX::XMFLOAT4 m_vColor = { 1, 1, 1, 1 };
		};
	}
}