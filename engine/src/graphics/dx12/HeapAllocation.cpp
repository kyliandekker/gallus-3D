#include "HeapAllocation.h"

// core includes
#include "core/Engine.h" 

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/dx12/DX12System2D.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// HeapAllocation
			//---------------------------------------------------------------------
			HeapAllocation::HeapAllocation(const D3D12_DESCRIPTOR_HEAP_DESC& a_Desc) : m_Type(a_Desc.Type)
			{
				Microsoft::WRL::ComPtr<ID3D12Device2>& device = core::ENGINE->GetDX12().GetDevice();
				HRESULT hr = device->CreateDescriptorHeap(&a_Desc, IID_PPV_ARGS(&m_pHeap));
				if (FAILED(hr))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to create descriptor heap.");
					return;
				}

				m_aAllocated = std::vector<bool>(a_Desc.NumDescriptors);
				m_iDescriptorSize = device->GetDescriptorHandleIncrementSize(m_Type);
			}

			//---------------------------------------------------------------------
			size_t HeapAllocation::Allocate()
			{
				for (size_t i = 0; i < m_aAllocated.size(); ++i)
				{
					if (!m_aAllocated[i])
					{
						m_aAllocated[i] = true;
						return i;
					}
				}

				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "No available descriptor heap slots!");
				throw std::runtime_error("Descriptor heap out of memory.");
			}

			//---------------------------------------------------------------------
			void HeapAllocation::Deallocate(size_t a_iIndex)
			{
				if (a_iIndex >= m_aAllocated.size())
				{
					LOG(LOGSEVERITY_WARNING, LOG_CATEGORY_DX12, "Deallocation index out of bounds.");
					return;
				}
				m_aAllocated[a_iIndex] = false;
			}

			//---------------------------------------------------------------------
			D3D12_CPU_DESCRIPTOR_HANDLE HeapAllocation::GetCPUDescriptorHandleForHeapStart()
			{
				return m_pHeap->GetCPUDescriptorHandleForHeapStart();
			}

			//---------------------------------------------------------------------
			D3D12_GPU_DESCRIPTOR_HANDLE HeapAllocation::GetGPUDescriptorHandleForHeapStart()
			{
				return m_pHeap->GetGPUDescriptorHandleForHeapStart();
			}

			//---------------------------------------------------------------------
			CD3DX12_GPU_DESCRIPTOR_HANDLE HeapAllocation::GetGPUHandle(size_t a_iIndex)
			{
				CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_pHeap->GetGPUDescriptorHandleForHeapStart());
				gpuHandle.Offset(static_cast<INT>(a_iIndex), m_iDescriptorSize);

				return gpuHandle;
			}

			//---------------------------------------------------------------------
			CD3DX12_CPU_DESCRIPTOR_HANDLE HeapAllocation::GetCPUHandle(size_t a_iIndex)
			{
				CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_pHeap->GetCPUDescriptorHandleForHeapStart());
				cpuHandle.Offset(static_cast<INT>(a_iIndex), m_iDescriptorSize);

				return cpuHandle;
			}

			//---------------------------------------------------------------------
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& HeapAllocation::GetHeap()
			{
				return m_pHeap;
			}
		}
	}
}