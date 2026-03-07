#pragma once

#include "DX12PCH.h"

// external
#include <vector>

namespace gallus::graphics::dx12
{
	//---------------------------------------------------------------------
	// DX12UploadBufferAllocator
	//---------------------------------------------------------------------
	/// <summary>
	/// Manages allocation of elements inside a GPU upload buffer.
	/// Can be used for transforms, skinning data, materials, etc.
	/// </summary>
	class DX12UploadBufferAllocator
	{
	public:
		/// <summary>
		/// Default constructor.
		/// </summary>
		DX12UploadBufferAllocator() = default;

		/// <summary>
		/// Initializes the upload buffer allocator.
		/// </summary>
		bool Initialize(
			ID3D12Device* a_pDevice,
			size_t a_iElementSize,
			size_t a_iElementCount,
			size_t a_iAlignment = 256
		);

		/// <summary>
		/// Allocates a free element index.
		/// </summary>
		size_t Allocate();

		/// <summary>
		/// Frees a previously allocated element.
		/// </summary>
		void Deallocate(size_t a_iIndex);

		/// <summary>
		/// Returns CPU pointer for writing data.
		/// </summary>
		void* GetCPUAddress(size_t a_iIndex);

		/// <summary>
		/// Returns GPU virtual address for binding.
		/// </summary>
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress(size_t a_iIndex) const;
	private:
		size_t Align(size_t a_iSize, size_t a_iAlignment);
	private:
		size_t m_iElementSize = 0;
		size_t m_iAlignedElementSize = 0;
		size_t m_iElementCount = 0;

		std::vector<bool> m_aAllocated;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_pBuffer;

		uint8_t* m_pMappedMemory = nullptr;

		D3D12_GPU_VIRTUAL_ADDRESS m_iBaseGPUAddress = 0;
	};
}