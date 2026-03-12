#include "DX12UploadBufferAllocator.h"

#include <stdexcept>

namespace gallus::graphics::dx12
{
	//---------------------------------------------------------------------
	// DX12UploadBufferAllocator
	//---------------------------------------------------------------------
	size_t DX12UploadBufferAllocator::Align(size_t a_iSize, size_t a_iAlignment)
	{
		return (a_iSize + a_iAlignment - 1) & ~(a_iAlignment - 1);
	}

	//---------------------------------------------------------------------
	bool DX12UploadBufferAllocator::Initialize(
		ID3D12Device* a_pDevice,
		size_t a_iElementSize,
		size_t a_iElementCount,
		size_t a_iAlignment)
	{
		m_iElementSize = a_iElementSize;
		m_iElementCount = a_iElementCount;

		m_iAlignedElementSize = Align(a_iElementSize, a_iAlignment);

		m_aAllocated.resize(a_iElementCount, false);

		UINT64 iBufferSize = m_iAlignedElementSize * a_iElementCount;

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = iBufferSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(a_pDevice->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pBuffer))))
		{
			return false;
		}

		m_iBaseGPUAddress = m_pBuffer->GetGPUVirtualAddress();

		m_pBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pMappedMemory));

		return true;
	}

	//---------------------------------------------------------------------
	size_t DX12UploadBufferAllocator::Allocate()
	{
		for (size_t i = 0; i < m_iElementCount; ++i)
		{
			if (!m_aAllocated[i])
			{
				m_aAllocated[i] = true;
				return i;
			}
		}

		throw std::runtime_error("DX12UploadBufferAllocator exhausted.");
	}

	//---------------------------------------------------------------------
	void DX12UploadBufferAllocator::Deallocate(size_t a_iIndex)
	{
		if (a_iIndex < 0)
		{
			return;
		}

		if (a_iIndex < m_iElementCount)
		{
			m_aAllocated[a_iIndex] = false;
		}
	}

	//---------------------------------------------------------------------
	void* DX12UploadBufferAllocator::GetCPUAddress(size_t a_iIndex)
	{
		return m_pMappedMemory + (a_iIndex * m_iAlignedElementSize);
	}

	//---------------------------------------------------------------------
	D3D12_GPU_VIRTUAL_ADDRESS DX12UploadBufferAllocator::GetGPUAddress(size_t a_iIndex) const
	{
		return m_iBaseGPUAddress + (a_iIndex * m_iAlignedElementSize);
	}
}