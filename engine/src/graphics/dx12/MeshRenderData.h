#pragma once

#include <memory>

// graphics
#include "graphics/dx12/DX12PCH.h"

namespace gallus::graphics::dx12
{
	class CommandList;

	class Texture;
	class Mesh;
	class Material;
	class Camera;
	class PixelShader;
	class VertexShader;
	class Transform;

	//---------------------------------------------------------------------
	// MeshRenderData
	//---------------------------------------------------------------------
	struct MeshRenderData
	{
		/// <summary>
		/// Renders the mesh.
		/// </summary>
		/// <param name="a_pCommandList">The command list used for rendering.</param>
		/// <param name="a_Transform">The transform of the mesh.</param>
		/// <param name="a_Camera">The camera.</param>
		void Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const graphics::dx12::Transform& a_Transform, uint16_t a_iTransformIndex, const graphics::dx12::Camera& a_Camera);

		std::weak_ptr<graphics::dx12::PixelShader> m_pPixelShader = {};
		std::weak_ptr<graphics::dx12::VertexShader> m_pVertexShader = {};
		std::weak_ptr<graphics::dx12::Mesh> m_pMesh = {};
		std::weak_ptr<graphics::dx12::Texture> m_pTexture = {};
		uint16_t m_iTextureRectIndex = 0;
		std::weak_ptr<graphics::dx12::Material> m_pMaterial = {};

		int16_t m_iSkinnedMeshIndex = 0;

		ID3D12PipelineState* m_pPipelineState = nullptr;
		ID3D12RootSignature* m_pRootSignature = nullptr;
	};
}