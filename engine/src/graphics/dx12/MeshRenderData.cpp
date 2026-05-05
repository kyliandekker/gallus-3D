#include "graphics/dx12/MeshRenderData.h"

#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/DX12UploadBufferAllocator.h"
#include "graphics/dx12/Camera.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Material.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Transform.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

namespace gallus::graphics::dx12
{
	//---------------------------------------------------------------------
	void MeshRenderData::Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const graphics::dx12::Transform& a_Transform, uint16_t a_iTransformIndex, const graphics::dx12::Camera& a_Camera)
	{
		if (!m_pPipelineState)
		{
			return;
		}

		if (!m_pRootSignature)
		{
			return;
		}

		DirectX::XMMATRIX mMatrix = a_Transform.GetWorldMatrixWithPivot();
		graphics::dx12::CameraType cameraType = graphics::dx12::CameraType::CameraType_World;

		const DirectX::XMMATRIX viewMatrix = a_Camera.GetViewMatrix(cameraType);
		const DirectX::XMMATRIX& projectionMatrix = a_Camera.GetProjectionMatrix(cameraType);

		const DirectX::XMMATRIX mvpMatrix = mMatrix * viewMatrix * projectionMatrix;

		a_pCommandList->GetCommandList()->SetPipelineState(m_pPipelineState);
		a_pCommandList->GetCommandList()->SetGraphicsRootSignature(m_pRootSignature);

		if (std::shared_ptr<graphics::dx12::Material> material = m_pMaterial.lock())
		{
			if (material->IsValid())
			{
				material->Bind(a_pCommandList);
			}
		}

		if (std::shared_ptr<graphics::dx12::Texture> tex = m_pTexture.lock())
		{
			if (tex->CanBeDrawn())
			{
				tex->Bind(a_pCommandList, m_iTextureRectIndex);
			}
		}

		if (std::shared_ptr<graphics::dx12::Mesh> mesh = m_pMesh.lock())
		{
			if (mesh->IsValid())
			{
				for (graphics::dx12::MeshPartData& meshData : mesh->GetMeshData())
				{
					{
						D3D12_GPU_VIRTUAL_ADDRESS gpuAddr = GetDX12System().GetSkinningDataAllocator()->GetGPUAddress(m_iSkinnedMeshIndex);
						a_pCommandList->GetCommandList()->SetGraphicsRootConstantBufferView(
							graphics::dx12::RootParameters::SKINNING_DATA,
							gpuAddr);
					}

					{
						graphics::dx12::ShaderTransform* pTransform =
							reinterpret_cast<graphics::dx12::ShaderTransform*>(
								GetDX12System()
								.GetTransformAllocator()
								->GetCPUAddress(a_iTransformIndex));

						pTransform->WorldViewProj = mvpMatrix;
						pTransform->WorldMatrix = mMatrix;

						a_pCommandList->GetCommandList()->SetGraphicsRootConstantBufferView(
							graphics::dx12::RootParameters::TRANSFORM,
							GetDX12System()
							.GetTransformAllocator()
							->GetGPUAddress(a_iTransformIndex));
					}

					mesh->RenderMeshData(meshData, a_pCommandList);
				}
			}
		}
	}
}