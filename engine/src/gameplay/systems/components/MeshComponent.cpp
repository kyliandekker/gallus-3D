#include "MeshComponent.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Material.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Transform.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/ShaderFactory.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/systems/TransformSystem.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// MeshComponent
		//---------------------------------------------------------------------
		void MeshComponent::SetDefaults(const gameplay::EntityID& a_EntityID)
		{
			Component::SetDefaults(a_EntityID);

			resources::ResourceAtlas& resourceAtlas = core::ENGINE->GetResourceAtlas();
			m_pTexture = resourceAtlas.GetDefaultTexture();
			m_pMesh = resourceAtlas.GetDefaultMesh();
			m_vColor = { 1, 1, 1, 1 };

			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
			if (transformSys.HasComponent(a_EntityID))
			{
				graphics::dx12::Transform transform = transformSys.GetComponent(a_EntityID).GetTransform();
				transform.SetPivot({ 0, 0, 0 });
			}
		}

		//---------------------------------------------------------------------
		void MeshComponent::Init()
		{
			// cache is in the init function of component.
			Component::Init();

			OnShadersChanged();

			// cache
			m_pRootSignature = core::ENGINE->GetDX12().GetRootSignature().Get();
			m_pTransformSystem = &m_pECS->GetSystem<gameplay::TransformSystem>();
		}

		//---------------------------------------------------------------------
		void MeshComponent::SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh)
		{
			m_pMesh = a_pMesh;
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

			auto ent = m_pECS->GetEntity(m_EntityID).lock();
			if (!ent || !ent->IsActive())
			{
				return;
			}

			graphics::dx12::Transform transform;
			if (m_pTransformSystem->HasComponent(a_EntityID))
			{
				transform = m_pTransformSystem->GetComponent(a_EntityID).GetTransform();
			}

			if (transform.GetCameraType() == graphics::dx12::CameraType_Screen)
			{
				if (core::ENGINE->GetDX12().GetDimensionDrawMode() != graphics::dx12::DimensionDrawMode::DimensionDrawMode_2D && core::ENGINE->GetDX12().GetDimensionDrawMode() != graphics::dx12::DimensionDrawMode::DimensionDrawMode_2D3D)
				{
					return;
				}
			}
			else if (transform.GetCameraType() == graphics::dx12::CameraType_World)
			{
				if (core::ENGINE->GetDX12().GetDimensionDrawMode() != graphics::dx12::DimensionDrawMode::DimensionDrawMode_3D && core::ENGINE->GetDX12().GetDimensionDrawMode() != graphics::dx12::DimensionDrawMode::DimensionDrawMode_2D3D)
				{
					return;
				}
			}

			const DirectX::XMMATRIX viewMatrix = a_Camera.GetViewMatrix(transform.GetCameraType());
			const DirectX::XMMATRIX& projectionMatrix = a_Camera.GetProjectionMatrix(transform.GetCameraType());

			DirectX::XMMATRIX mMatrix = transform.GetWorldMatrixWithPivot();
			DirectX::XMMATRIX mvpMatrix = mMatrix * viewMatrix * projectionMatrix;

			if (auto material = core::ENGINE->GetResourceAtlas().GetDefaultMaterial().lock())
			{
				material->Bind(a_pCommandList);
			}

			if (auto material = m_pMaterial.lock())
			{
				if (material->IsValid())
				{
					material->Bind(a_pCommandList);
				}
			}

			if (auto tex = m_pTexture.lock())
			{
				if (tex->CanBeDrawn())
				{
					tex->Bind(a_pCommandList, m_iTextureIndex);
				}
			}

			a_pCommandList->GetCommandList()->SetPipelineState(m_pPipelineState);
			a_pCommandList->GetCommandList()->SetGraphicsRootSignature(m_pRootSignature);

			if (auto mesh = m_pMesh.lock())
			{
				if (mesh->IsValid())
				{
					mesh->Render(a_pCommandList, mvpMatrix, mMatrix);
				}
			}
		}

		//---------------------------------------------------------------------
		void MeshComponent::OnShadersChanged()
		{
			m_pPipelineState = graphics::dx12::PipelineStateCache::GetOrCreate(m_pPixelShader, m_pVertexShader, DXGI_FORMAT_D32_FLOAT);
		}

		//---------------------------------------------------------------------
		void MeshComponent::SetTextureIndex(uint16_t a_iMeshIndex)
		{
			size_t numMeshRects = 0;
			if (auto tex = m_pTexture.lock())
			{
				numMeshRects = tex->GetSpriteRectsSize() - 1;
			}
			if (a_iMeshIndex < 0)
			{
				a_iMeshIndex = 0;
			}
			else if (a_iMeshIndex > numMeshRects)
			{
				a_iMeshIndex = static_cast<uint16_t>(numMeshRects);
			}
			m_iTextureIndex = a_iMeshIndex;
		}

		//---------------------------------------------------------------------
		void MeshComponent::SetMaterial(std::weak_ptr<graphics::dx12::Material> a_pMaterial)
		{
			m_pMaterial = a_pMaterial;
		}
	}
}