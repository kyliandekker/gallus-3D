#include "MeshComponent.h"

// core
#include "core/Engine.h"

// graphics
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
#include "graphics/dx12/ShaderFactory.h"

// resources
#include "resources/ResourceAtlas.h"
#include "resources/SrcData.h"

// gameplay
#include "gameplay/Entity.h"
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/systems/TransformSystem.h"

// logger
#include "logger/Logger.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// MeshComponent
	//---------------------------------------------------------------------
	void MeshComponent::SetDefaults(const gameplay::EntityID& a_EntityID)
	{
		Component::SetDefaults(a_EntityID);

		resources::ResourceAtlas* resourceAtlas = GetEngine().GetResourceAtlas();
		if (!resourceAtlas)
		{
			return;
		}

		m_pTexture = resourceAtlas->GetDefaultTexture();
		m_pMesh = resourceAtlas->GetDefaultMesh();
		m_pMaterial = resourceAtlas->GetDefaultMaterial();

		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		TransformSystem* transformSys = ecs->GetSystem<TransformSystem>();
		if (gameplay::TransformComponent* transform = transformSys->TryGetComponent(a_EntityID))
		{
			transform->GetTransform().SetPivot({ 0, 0, 0 });
		}

		m_iSkinnedMeshIndex = 0;
	}

	//---------------------------------------------------------------------
	void MeshComponent::Init()
	{
		// cache is in the init function of component.
		Component::Init();

		OnShadersChanged();

		// cache
		graphics::dx12::DX12System& dx12 = GetDX12System();

		m_pDX12System = &dx12;
		m_pRootSignature = dx12.GetRootSignature().Get();
		m_pTransformSystem = m_pECS->GetSystem<gameplay::TransformSystem>();
	}

	//---------------------------------------------------------------------
	void MeshComponent::SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh)
	{
		m_pMesh = a_pMesh;

		if (m_iSkinnedMeshIndex < 1)
		{
			m_iSkinnedMeshIndex = GetDX12System().GetSkinningDataAllocator()->Allocate();
		}
	}

	//---------------------------------------------------------------------
	void MeshComponent::SetTexture(std::weak_ptr<graphics::dx12::Texture> a_pTexture)
	{
		m_pTexture = a_pTexture;
	}

	//---------------------------------------------------------------------
	void MeshComponent::Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera)
	{
		if (!m_pECS)
		{
			return;
		}

		if (!m_pPipelineState)
		{
			return;
		}

		if (!m_pRootSignature)
		{
			return;
		}

		std::shared_ptr<gameplay::Entity> ent = m_pECS->GetEntity(m_EntityID).lock();
		if (!ent || !ent->IsActive())
		{
			return;
		}

		if (!m_pDX12System)
		{
			return;
		}

		DirectX::XMMATRIX mMatrix = DirectX::XMMatrixIdentity();
		int transformIndex = 0;
		graphics::dx12::CameraType cameraType = graphics::dx12::CameraType::CameraType_World;

		if (m_pTransformSystem)
		{
			if (gameplay::TransformComponent* transformComponent = m_pTransformSystem->TryGetComponent(a_EntityID))
			{
				// Respect camera type dimension filtering
				if (transformComponent->GetTransform().GetCameraType() == graphics::dx12::CameraType_Screen)
				{
					if (m_pDX12System->GetDimensionDrawMode() != graphics::dx12::DimensionDrawMode::DimensionDrawMode_2D &&
						m_pDX12System->GetDimensionDrawMode() != graphics::dx12::DimensionDrawMode::DimensionDrawMode_2D3D)
					{
						return;
					}
				}
				else if (transformComponent->GetTransform().GetCameraType() == graphics::dx12::CameraType_World)
				{
					if (m_pDX12System->GetDimensionDrawMode() != graphics::dx12::DimensionDrawMode::DimensionDrawMode_3D &&
						m_pDX12System->GetDimensionDrawMode() != graphics::dx12::DimensionDrawMode::DimensionDrawMode_2D3D)
					{
						return;
					}
				}

				mMatrix = transformComponent->GetTransform().GetWorldMatrixWithPivot();
				transformIndex = transformComponent->GetTransformIndex();
				cameraType = transformComponent->GetTransform().GetCameraType();
			}
		}

		const DirectX::XMMATRIX viewMatrix = a_Camera.GetViewMatrix(cameraType);
		const DirectX::XMMATRIX& projectionMatrix = a_Camera.GetProjectionMatrix(cameraType);

		const DirectX::XMMATRIX mvpMatrix = mMatrix * viewMatrix * projectionMatrix;

		resources::ResourceAtlas* resourceAtlas = GetEngine().GetResourceAtlas();
		if (!resourceAtlas)
		{
			return;
		}

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
							RootParameters::SKINNING_DATA,
							gpuAddr);

					}

					{
						ShaderTransform* pTransform =
							reinterpret_cast<ShaderTransform*>(
								GetDX12System()
								.GetTransformAllocator()
								->GetCPUAddress(transformIndex));

						pTransform->WorldViewProj = mvpMatrix;
						pTransform->WorldMatrix = mMatrix;

						a_pCommandList->GetCommandList()->SetGraphicsRootConstantBufferView(
							RootParameters::TRANSFORM,
							GetDX12System()
							.GetTransformAllocator()
							->GetGPUAddress(transformIndex));
					}

					mesh->RenderMeshData(meshData, a_pCommandList);
				}
			}
		}
	}

	//---------------------------------------------------------------------
	void MeshComponent::OnShadersChanged()
	{
		m_pPipelineState = graphics::dx12::PipelineStateCache::GetOrCreate(m_pPixelShader, m_pVertexShader, DXGI_FORMAT_D32_FLOAT);
	}

	//---------------------------------------------------------------------
	void MeshComponent::SetTextureRectIndex(uint16_t a_iRectIndex)
	{
		size_t numMeshRects = 0;
		if (std::shared_ptr<graphics::dx12::Texture> tex = m_pTexture.lock())
		{
			numMeshRects = tex->GetTextureRectsSize() - 1;
		}
		if (a_iRectIndex < 0)
		{
			a_iRectIndex = 0;
		}
		else if (a_iRectIndex > numMeshRects)
		{
			a_iRectIndex = static_cast<uint16_t>(numMeshRects);
		}
		m_iTextureRectIndex = a_iRectIndex;
	}

	//---------------------------------------------------------------------
	void MeshComponent::SetMaterial(std::weak_ptr<graphics::dx12::Material> a_pMaterial)
	{
		m_pMaterial = a_pMaterial;
	}
}