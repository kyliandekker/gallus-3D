#include "SpriteComponent.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/Camera.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Transform.h"
#include "graphics/dx12/Material.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

#include "graphics/dx12/ShaderFactory.h"

// resources
#include "resources/ResourceAtlas.h"

// gameplay
#include "gameplay/Entity.h"
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/systems/TransformSystem.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// SpriteComponent
	//---------------------------------------------------------------------
	void SpriteComponent::SetDefaults(const gameplay::EntityID& a_EntityID)
	{
		Component::SetDefaults(a_EntityID);

		resources::ResourceAtlas* resourceAtlas = GetEngine().GetResourceAtlas();
		if (!resourceAtlas)
		{
			return;
		}

		m_pTexture = resourceAtlas->GetDefaultTexture();
		m_pMesh = resourceAtlas->LoadMesh("square");
	}

	//---------------------------------------------------------------------
	void SpriteComponent::Init()
	{
		// cache is in the init function of component.
		Component::Init();

		OnShadersChanged();

		// cache
		graphics::dx12::DX12System& dx12 = GetDX12System();

		m_pDX12System = &dx12;
		m_pRootSignature = dx12.GetRootSignature().Get();
		m_pTransformSystem = m_pECS->GetSystem<gameplay::TransformSystem>();

		OnOrderChanged();
	}

	//---------------------------------------------------------------------
	void SpriteComponent::SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh)
	{
		m_pMesh = a_pMesh;
	}

	//---------------------------------------------------------------------
	void SpriteComponent::SetTexture(std::weak_ptr<graphics::dx12::Texture> a_pTexture)
	{
		m_pTexture = a_pTexture;
	}

	//---------------------------------------------------------------------
	void SpriteComponent::Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera)
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

		// TODO: Possible to cache this and improve performance.
		std::shared_ptr<gameplay::Entity> ent = m_pECS->GetEntity(m_EntityID).lock();
		if (!ent || !ent->IsActive())
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

		DirectX::XMMATRIX mvpMatrix;
		if (m_bIsStatic)
		{
			mvpMatrix = mMatrix * projectionMatrix;
		}
		else
		{
			mvpMatrix = mMatrix * viewMatrix * projectionMatrix;
		}

		resources::ResourceAtlas* resourceAtlas = GetEngine().GetResourceAtlas();
		if (!resourceAtlas)
		{
			return;
		}

		if (std::shared_ptr<graphics::dx12::Material> material = resourceAtlas->GetDefaultMaterial().lock())
		{
			material->Bind(a_pCommandList);
		}

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
				tex->Bind(a_pCommandList, m_iTextureIndex);
			}
		}

		a_pCommandList->GetCommandList()->SetPipelineState(m_pPipelineState);
		a_pCommandList->GetCommandList()->SetGraphicsRootSignature(m_pRootSignature);

		if (std::shared_ptr<graphics::dx12::Mesh> mesh = m_pMesh.lock())
		{
			if (mesh->IsValid())
			{
				mesh->Render(a_pCommandList, transformIndex, mvpMatrix, mMatrix);
			}
		}
	}
	
	//---------------------------------------------------------------------
	void SpriteComponent::OnOrderChanged()
	{
		m_pDX12System->ReorderSpriteComponents();
	}
	
	//---------------------------------------------------------------------
	void SpriteComponent::SetTextureIndex(int8_t a_iTextureIndex)
	{
		size_t numTextureRects = 0;
		if (std::shared_ptr<graphics::dx12::Texture> tex = m_pTexture.lock())
		{
			numTextureRects = tex->GetTextureRectsSize() - 1;
		}
		if (a_iTextureIndex < 0)
		{
			a_iTextureIndex = 0;
		}
		else if (a_iTextureIndex > numTextureRects)
		{
			a_iTextureIndex = numTextureRects;
		}
		m_iTextureIndex = a_iTextureIndex;
	}

	//---------------------------------------------------------------------
	void SpriteComponent::OnShadersChanged()
	{
		m_pPipelineState = graphics::dx12::PipelineStateCache::GetOrCreate(m_pPixelShader, m_pVertexShader, DXGI_FORMAT_D32_FLOAT);
	}
}