#pragma once

// graphics
#include "DX12PCH.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// CommandList
			//---------------------------------------------------------------------
			/// <summary>
			/// A wrapper for an ID3D12GraphicsCommandList2 to manage DirectX 12 command 
			/// list actions.
			/// </summary>
			class CommandList
			{
			public:
				CommandList() = default;

				/// <summary>
				/// Returns a pointer to the command list.
				/// </summary>
				/// <returns>The command list.</returns>
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>& GetCommandList();

				/// <summary>
				/// Creates a command list.
				/// </summary>
				/// <param name="a_pAllocator">The command allocator.</param>
				/// <param name="a_CommandListType">The command list type (direct, copy, etc).</param>
				/// <returns>True if the command list was created, false otherwise.</returns>
				bool CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& a_pAllocator, D3D12_COMMAND_LIST_TYPE a_CommandListType);

				/// <summary>
				/// Updates the buffer resource.
				/// </summary>
				/// <param name="a_pDestinationResource">The resource that needs to be updated.</param>
				/// <param name="a_pIntermediateResource">The resource that needs to be updated in between.</param>
				/// <param name="a_NumElements">The number of elements to update the resource with.</param>
				/// <param name="a_ElementSize">The element size to update the resource with.</param>
				/// <param name="a_BufferData">Pointer to the data that will be put in the resource.</param>
				/// <param name="a_Flags">Flags that will be put in the resource.</param>
				void UpdateBufferResource(ID3D12Resource** a_pDestinationResource, ID3D12Resource** a_pIntermediateResource, size_t a_NumElements, size_t a_ElementSize, const void* a_BufferData, D3D12_RESOURCE_FLAGS a_Flags = D3D12_RESOURCE_FLAG_NONE);
			private:
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_pCommandList;
			};
		}
	}
}