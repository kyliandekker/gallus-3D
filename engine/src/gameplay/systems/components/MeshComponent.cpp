#include "MeshComponent.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/ShaderBind.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Transform.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

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

			m_pShaderBind = core::ENGINE->GetResourceAtlas().LoadShaderBind("defaultShaderBind");
			m_pTexture = core::ENGINE->GetResourceAtlas().GetDefaultTexture();
			m_pMesh = core::ENGINE->GetResourceAtlas().GetDefaultMesh();
			m_vColor = { 1, 1, 1, 1 };

			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
			if (transformSys.HasComponent(a_EntityID))
			{
				graphics::dx12::Transform transform = transformSys.GetComponent(a_EntityID).GetTransform();
				transform.SetPivot({ 0, 0, 0 });
			}
		}

		//---------------------------------------------------------------------
		void MeshComponent::SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh)
		{
			m_pMesh = a_pMesh;
		}

		//---------------------------------------------------------------------
		void MeshComponent::SetShader(std::weak_ptr<graphics::dx12::ShaderBind> a_pShaderBind)
		{
			m_pShaderBind = a_pShaderBind;
		}

		//---------------------------------------------------------------------
		void MeshComponent::SetTexture(std::weak_ptr<graphics::dx12::Texture> a_pTexture)
		{
			m_pTexture = a_pTexture;
		}

		//---------------------------------------------------------------------
		void MeshComponent::Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera)
		{
			auto entity = core::ENGINE->GetECS().GetEntity(m_EntityID);
			if (!entity || !entity->IsActive())
			{
				return;
			}

			graphics::dx12::Transform transform;
			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
			if (transformSys.HasComponent(a_EntityID))
			{
				transform = transformSys.GetComponent(a_EntityID).GetTransform();
			}

			if (transform.GetCameraType() == graphics::dx12::CameraType_Screen)
			{
				if (core::ENGINE->GetDX12().GetCameraIsolationMode() != graphics::dx12::CameraIsolationMode::CameraIsolationMode_2D && core::ENGINE->GetDX12().GetCameraIsolationMode() != graphics::dx12::CameraIsolationMode::CameraIsolationMode_2D3D)
				{
					return;
				}
			}
			else if (transform.GetCameraType() == graphics::dx12::CameraType_World)
			{
				if (core::ENGINE->GetDX12().GetCameraIsolationMode() != graphics::dx12::CameraIsolationMode::CameraIsolationMode_3D && core::ENGINE->GetDX12().GetCameraIsolationMode() != graphics::dx12::CameraIsolationMode::CameraIsolationMode_2D3D)
				{
					return;
				}
			}

			const DirectX::XMMATRIX viewMatrix = a_Camera.GetViewMatrix(transform.GetCameraType());
			const DirectX::XMMATRIX& projectionMatrix = a_Camera.GetProjectionMatrix(transform.GetCameraType());

			DirectX::XMMATRIX mvpMatrix = transform.GetWorldMatrixWithPivot() * viewMatrix * projectionMatrix;

			if (!core::ENGINE->GetECS().GetEntity(a_EntityID))
			{
				return;
			}

			if (!core::ENGINE->GetECS().GetEntity(a_EntityID)->IsActive())
			{
				return;
			}

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

			if (auto shaderBind = m_pShaderBind.lock())
			{
				if (shaderBind->IsValid())
				{
					shaderBind->Bind(a_pCommandList);
				}
			}

			if (auto mesh = m_pMesh.lock())
			{
				if (mesh->IsValid())
				{
					mesh->Render(a_pCommandList, mvpMatrix);
				}
			}
		}

		/// <summary>
		/// Sets the sprite index.
		/// </summary>
		/// <param name="a_iMeshIndex">The index the sprite should have.</param>
		void MeshComponent::SetTextureIndex(int8_t a_iMeshIndex)
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
				a_iMeshIndex = numMeshRects;
			}
			m_iTextureIndex = a_iMeshIndex;
		}
	}
}