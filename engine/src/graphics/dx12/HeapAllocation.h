#pragma once

#include "DX12PCH.h"

#include <vector>
#include <mutex>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// HeapAllocation
			//---------------------------------------------------------------------
			/// <summary>
			/// Manages allocation of descriptor handles from a DirectX 12 descriptor heap.
			/// Provides thread-safe allocation and deallocation, as well as access to handle offsets.
			/// </summary>
			class HeapAllocation
			{
			public:
				/// <summary>
				/// Default constructor. Does not create a descriptor heap.
				/// </summary>
				HeapAllocation() = default;

				/// <summary>
				/// Constructs a new descriptor heap and initializes allocation tracking.
				/// </summary>
				/// <param name="a_Desc">The descriptor heap description to create.</param>
				HeapAllocation(const D3D12_DESCRIPTOR_HEAP_DESC& a_Desc);

				/// <summary>
				/// Allocates a free descriptor slot from the heap.
				/// </summary>
				size_t Allocate();

				/// <summary>
				/// Frees a previously allocated descriptor slot.
				/// </summary>
				/// <param name="a_iIndex">The index of the descriptor to free.</param>
				void Deallocate(size_t a_iIndex);

				/// <summary>
				/// Gets the starting CPU descriptor handle for the heap.
				/// </summary>
				/// <returns>The CPU descriptor handle at the start of the heap.</returns>
				D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart();

				/// <summary>
				/// Gets the starting GPU descriptor handle for the heap.
				/// </summary>
				/// <returns>The GPU descriptor handle at the start of the heap.</returns>
				D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart();

				/// <summary>
				/// Gets a GPU descriptor handle at a specific index.
				/// </summary>
				/// <param name="a_iIndex">The descriptor index.</param>
				/// <returns>The GPU descriptor handle at the given index.</returns>
				CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(size_t a_iIndex);

				/// <summary>
				/// Gets a CPU descriptor handle at a specific index.
				/// </summary>
				/// <param name="a_iIndex">The descriptor index.</param>
				/// <returns>The CPU descriptor handle at the given index.</returns>
				CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(size_t a_iIndex);

				/// <summary>
				/// Gets the underlying descriptor heap object.
				/// </summary>
				/// <returns>A smart pointer to the descriptor heap.</returns>
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetHeap();
			private:
				UINT m_iDescriptorSize = 0;
				D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pHeap;

				std::vector<bool> m_aAllocated;
			};
		}
	}
}