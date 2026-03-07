#include "TransformComponent.h"

// graphics
#include "graphics/dx12/ShaderDefs.h"
#include "graphics/dx12/DX12Resource.h"

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
	void TransformComponent::Init()
	{
		m_pTransformBuffer = std::make_shared<graphics::dx12::DX12Resource>();

		// Assume device is ID3D12Device*, uploadHeap is the heap for dynamic buffers
		ShaderTransform transformData;

		// Fill initial data
		transformData.WorldMatrix = DirectX::XMMatrixIdentity();
		transformData.WorldViewProj = DirectX::XMMatrixIdentity();

		// Create upload buffer
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC bufDesc = {};
		bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufDesc.Alignment = 0;
		bufDesc.Width = sizeof(ShaderTransform);
		bufDesc.Height = 1;
		bufDesc.DepthOrArraySize = 1;
		bufDesc.MipLevels = 1;
		bufDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufDesc.SampleDesc.Count = 1;
		bufDesc.SampleDesc.Quality = 0;
		bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (!m_pTransformBuffer->SetResourceData("TransformBuffer", bufDesc, heapProps, D3D12_RESOURCE_STATE_GENERIC_READ))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed resizing depth buffer: Failed creating committed resource.");
			return;
		}
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
    std::weak_ptr<graphics::dx12::DX12Resource> TransformComponent::GetMappedTransformBuffer(const DirectX::XMMATRIX& a_MVP, const DirectX::XMMATRIX& a_M)
    {
		ShaderTransform* pMappedData = nullptr;
		m_pTransformBuffer->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&pMappedData));

		// Update the MVP matrix
		pMappedData->WorldViewProj = a_MVP;
		pMappedData->WorldMatrix = a_M;
		m_pTransformBuffer->GetResource()->Unmap(0, nullptr);

        return m_pTransformBuffer;
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