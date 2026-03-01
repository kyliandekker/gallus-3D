#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <memory>

// graphics
#include "graphics/dx12/DX12PCH.h"

// resources
#include "resources/AssetType.h"

namespace gallus::graphics::dx12
{
	class DX12System;
	class CommandList;

	class Transform;
	class Texture;
	class Mesh;
	class Camera;
	class PixelShader;
	class VertexShader;
	class Material;
}
namespace gallus::gameplay
{
	struct EntityID;
	class TransformSystem;

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
		/// Initializes the component in runtime.
		/// </summary>
		void Init() override;

		/// <summary>
		/// Sets the mesh used by the mesh component.
		/// </summary>
		/// <param name="a_Mesh">Reference to the mesh that the mesh component will use.</param>
		void SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh);

		/// <summary>
		/// Sets the texture used by the mesh component.
		/// </summary>
		/// <param name="a_Texture">Reference to the texture that the mesh component will use.</param>
		void SetTexture(std::weak_ptr < graphics::dx12::Texture> a_pTexture);

		/// <summary>
		/// Sets the sprite index.
		/// </summary>
		/// <param name="a_iTextureIndex">The index the sprite should have.</param>
		void SetTextureIndex(int8_t a_iTextureIndex);
		
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
		
		std::weak_ptr<graphics::dx12::PixelShader> m_pPixelShader = {};
		std::weak_ptr<graphics::dx12::VertexShader> m_pVertexShader = {};
		std::weak_ptr<graphics::dx12::Mesh> m_pMesh = {};
		std::weak_ptr<graphics::dx12::Texture> m_pTexture = {};
		uint16_t m_iTextureIndex = 0;
		std::weak_ptr<graphics::dx12::Material> m_pMaterial = {};

		void OnShadersChanged();

		BEGIN_SERIALIZE_PARENT(SpriteComponent, Component)
			SERIALIZE_FIELD(m_pPixelShader, "Pixel Shader", "Pointer to the pixel shader asset used for rendering this object. Can be nullptr if no specific pixel shader is assigned.",
				.type = FieldSerializationType::FieldSerializationType_EngineResource,
				.assetType = resources::AssetType::PixelShader,
				.onChangeFunc = MakeOnChangeFunc(&SpriteComponent::OnShadersChanged))
			SERIALIZE_FIELD(m_pVertexShader, "Vertex Shader", "Pointer to the vertex shader asset used for rendering this object. Can be nullptr if no specific vertex shader is assigned.",
				.type = FieldSerializationType::FieldSerializationType_EngineResource,
				.assetType = resources::AssetType::VertexShader,
				.onChangeFunc = MakeOnChangeFunc(&SpriteComponent::OnShadersChanged))
		 	SERIALIZE_FIELD(m_bIsStatic, "Static", "Determines whether the sprite should stick on the screen or not.",
		 		.type = FieldSerializationType::FieldSerializationType_Bool)
		 	SERIALIZE_FIELD(m_iOrder, "Order", "Determines what sprites overlap other sprites.",
		 		.type = FieldSerializationType::FieldSerializationType_Int8,
		 		.onChangeFunc = MakeOnChangeFunc(&SpriteComponent::OnOrderChanged))
			SERIALIZE_FIELD(m_pMaterial, "Material", "Pointer to the material asset used by this Mesh. Can be nullptr if no material is assigned. Determines the visual appearance of the Mesh.",
				.type = FieldSerializationType::FieldSerializationType_EngineResource,
				.assetType = resources::AssetType::Material)
			SERIALIZE_FIELD(m_pMaterial, "MaterialView", "",
				.type = FieldSerializationType::FieldSerializationType_ObjectPtr,
				.assetType = resources::AssetType::Material,
				.serialize = false,
				.indent = 1)
		 	SERIALIZE_FIELD(m_pTexture, "Texture", "Pointer to the texture asset used by this sprite. Can be nullptr if no texture is assigned. Determines the visual appearance of the sprite.",
		 		.type = FieldSerializationType::FieldSerializationType_EngineResource,
		 		.assetType = resources::AssetType::Texture)
		 	SERIALIZE_FIELD(m_iTextureIndex, "Texture Index", "Index of the sprite within a texture atlas. Used when the texture contains multiple sprites to select which one is displayed.",
		 		.type = FieldSerializationType::FieldSerializationType_Int16)
		 	SERIALIZE_FIELD(m_pTexture, "Texture Preview", "",
		 		.type = FieldSerializationType::FieldSerializationType_TexturePreview,
		 		.relatedIndexFieldOffset = offsetof(SpriteComponent, m_iTextureIndex))
		END_SERIALIZE(SpriteComponent)

		// cache
		ID3D12PipelineState* m_pPipelineState = nullptr;
		ID3D12RootSignature* m_pRootSignature = nullptr;
		gameplay::TransformSystem* m_pTransformSystem = nullptr;
		graphics::dx12::DX12System* m_pDX12System = nullptr;
	};
}