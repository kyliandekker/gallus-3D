#include "SpriteComponent.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/ShaderBind.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Transform.h"
#include "graphics/dx12/Material.h"
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
		// SpriteComponent
		//---------------------------------------------------------------------
		void SpriteComponent::SetDefaults(const gameplay::EntityID& a_EntityID)
		{
			Component::SetDefaults(a_EntityID);

			m_pShaderBind = core::ENGINE->GetResourceAtlas().LoadShaderBind("defaultShaderBind");
			m_pSprite = core::ENGINE->GetResourceAtlas().GetDefaultTexture();
			m_pMesh = core::ENGINE->GetResourceAtlas().LoadMesh("square");
			m_vColor = { 1, 1, 1, 1 };
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh)
		{
			m_pMesh = a_pMesh;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetShader(std::weak_ptr<graphics::dx12::ShaderBind> a_pShaderBind)
		{
			m_pShaderBind = a_pShaderBind;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetTexture(std::weak_ptr<graphics::dx12::Texture> a_pTexture)
		{
			m_pSprite = a_pTexture;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera)
		{
			auto ent = core::ENGINE->GetECS().GetEntity(m_EntityID).lock();
			if (!ent || !ent->IsActive())
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
			DirectX::XMMATRIX mMatrix = transform.GetWorldMatrixWithPivot();

			DirectX::XMMATRIX mvpMatrix;
			if (m_bIsStatic)
			{
				mvpMatrix = mMatrix * projectionMatrix;
			}
			else
			{
				mvpMatrix = mMatrix * viewMatrix * projectionMatrix;
			}

			if (auto material = core::ENGINE->GetResourceAtlas().GetDefaultMaterial().lock())
			{
				material->Bind(a_pCommandList);
			}

			if (auto tex = m_pSprite.lock())
			{
				if (tex->CanBeDrawn())
				{
					tex->Bind(a_pCommandList, m_iSpriteIndex);
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
					mesh->Render(a_pCommandList, mvpMatrix, mMatrix);
				}
			}
		}
		
		//---------------------------------------------------------------------
		void SpriteComponent::OnOrderChanged()
		{
			core::ENGINE->GetDX12().ReorderSpriteComponents();
		}
		
		//---------------------------------------------------------------------
		void SpriteComponent::SetSpriteIndex(int8_t a_iSpriteIndex)
		{
			size_t numSpriteRects = 0;
			if (auto tex = m_pSprite.lock())
			{
				numSpriteRects = tex->GetSpriteRectsSize() - 1;
			}
			if (a_iSpriteIndex < 0)
			{
				a_iSpriteIndex = 0;
			}
			else if (a_iSpriteIndex > numSpriteRects)
			{
				a_iSpriteIndex = numSpriteRects;
			}
			m_iSpriteIndex = a_iSpriteIndex;
		}
	}
}