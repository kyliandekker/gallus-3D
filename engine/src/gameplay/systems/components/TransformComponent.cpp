#include "TransformComponent.h"

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/DX12UploadBufferAllocator.h"
#include "graphics/dx12/ShaderDefs.h"

// logger
#include "logger/Logger.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// TransformComponent
	//---------------------------------------------------------------------
	graphics::dx12::Transform& TransformComponent::GetTransform()
	{
		return m_Transform;
	}

	//---------------------------------------------------------------------
	TransformComponent::~TransformComponent()
	{
		GetDX12System().GetTransformAllocator()->Deallocate(m_iTransformIndex);
	}

	//---------------------------------------------------------------------
	void TransformComponent::Init()
	{
		m_iTransformIndex = GetDX12System().GetTransformAllocator()->Allocate();
		Component::Init();
	}

	//---------------------------------------------------------------------
	void TransformComponent::Translate(const DirectX::XMFLOAT3& a_vTranslation)
	{
		m_vTranslation = {
			m_vTranslation.x + a_vTranslation.x,
			m_vTranslation.y + a_vTranslation.y,
			m_vTranslation.z + a_vTranslation.z
		};
	}

	//---------------------------------------------------------------------
	void TransformComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
	{
		if (m_vTranslation.x == 0.0f && m_vTranslation.y == 0.0)
		{
			return;
		}

		DirectX::XMFLOAT3 newPos = {
			m_Transform.GetPosition().x + m_vTranslation.x,
			m_Transform.GetPosition().y + m_vTranslation.y,
			m_Transform.GetPosition().z + m_vTranslation.z
		};
		m_Transform.SetPosition(newPos);

		m_vTranslation = {};
	}
}