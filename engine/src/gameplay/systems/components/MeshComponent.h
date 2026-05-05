#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <memory>

// graphics
#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/MeshRenderData.h"

// resources
#include "resources/AssetType.h"

namespace gallus::graphics::dx12
{
	class CommandList;

	class Texture;
	class Mesh;
	class Material;
	class Camera;

	class DX12System;
}
namespace gallus::gameplay
{
	struct EntityID;
	class TransformSystem;

	//---------------------------------------------------------------------
	// MeshComponent
	//---------------------------------------------------------------------
	class MeshComponent : public Component
	{
	public:
		~MeshComponent();

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
		void SetTexture(std::weak_ptr<graphics::dx12::Texture> a_pTexture);

		/// <summary>
		/// Sets the texture index.
		/// </summary>
		/// <param name="a_iTextureIndex">The index the texture should have.</param>
		void SetTextureRectIndex(uint16_t a_iTextureIndex);

		/// <summary>
		/// Sets the material.
		/// </summary>
		/// <param name="a_pMaterial">The material the mesh should have.</param>
		void SetMaterial(std::weak_ptr<graphics::dx12::Material> a_pMaterial);

		/// <summary>
		/// Renders the mesh.
		/// </summary>
		/// <param name="a_pCommandList">The command list used for rendering.</param>
		/// <param name="a_EntityID">The entity ID to get the data from.</param>
		/// <param name="a_Camera">The camera.</param>
		void Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera);
	private:
		void OnShadersChanged();

		BEGIN_SERIALIZE_PARENT(MeshComponent, Component)
			SERIALIZE_FIELD(m_MeshRenderData.m_pPixelShader, "Pixel Shader", "Pointer to the pixel shader asset used for rendering this object. Can be nullptr if no specific pixel shader is assigned.",
				.type = FieldSerializationType::FieldSerializationType_EngineResource,
				.assetType = resources::AssetType::PixelShader,
				.onChangeFunc = MakeOnChangeFunc(&MeshComponent::OnShadersChanged))
			SERIALIZE_FIELD(m_MeshRenderData.m_pVertexShader, "Vertex Shader", "Pointer to the vertex shader asset used for rendering this object. Can be nullptr if no specific vertex shader is assigned.",
				.type = FieldSerializationType::FieldSerializationType_EngineResource,
				.assetType = resources::AssetType::VertexShader,
				.onChangeFunc = MakeOnChangeFunc(&MeshComponent::OnShadersChanged))
			SERIALIZE_FIELD(m_MeshRenderData.m_pMaterial, "Material", "Pointer to the material asset used by this Mesh. Can be nullptr if no material is assigned. Determines the visual appearance of the Mesh.",
				.type = FieldSerializationType::FieldSerializationType_EngineResource,
				.assetType = resources::AssetType::Material)
			SERIALIZE_FIELD(m_MeshRenderData.m_pMaterial, "MaterialView", "",
				.type = FieldSerializationType::FieldSerializationType_ObjectPtr,
				.assetType = resources::AssetType::Material,
				.serialize = false,
				.indent = 1)
			SERIALIZE_FIELD(m_MeshRenderData.m_pMesh, "Mesh", "Pointer to the texture asset used by this Mesh. Can be nullptr if no mesh is assigned. Determines the visual appearance of the Mesh.",
				.type = FieldSerializationType::FieldSerializationType_EngineResource,
				.assetType = resources::AssetType::Mesh)
			SERIALIZE_FIELD(m_MeshRenderData.m_pTexture, "Texture", "Pointer to the mesh asset used by this Mesh. Can be nullptr if no texture is assigned. Determines the visual appearance of the Mesh.",
				.type = FieldSerializationType::FieldSerializationType_EngineResource,
				.assetType = resources::AssetType::Texture)
			SERIALIZE_FIELD(m_MeshRenderData.m_iTextureRectIndex, "Texture Index", "Index of the Mesh within a texture atlas. Used when the texture contains multiple rects to select which one is displayed.",
				.type = FieldSerializationType::FieldSerializationType_Int16)
			SERIALIZE_FIELD(m_MeshRenderData.m_pTexture, "Texture Preview", "",
				.type = FieldSerializationType::FieldSerializationType_TexturePreview,
				.relatedIndexFieldOffset = offsetof(MeshComponent, m_MeshRenderData.m_iTextureRectIndex))
		END_SERIALIZE(MeshComponent)

		// cache
		gameplay::TransformSystem* m_pTransformSystem = nullptr;
		graphics::dx12::DX12System* m_pDX12System = nullptr;

		graphics::dx12::MeshRenderData m_MeshRenderData;
	};
}