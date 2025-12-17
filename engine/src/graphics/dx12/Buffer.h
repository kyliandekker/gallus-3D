#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

// external
#include <string>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// Buffer
			//---------------------------------------------------------------------
			class Buffer : public DX12Resource
			{
			public:
				/// <summary>
				/// Constructs a buffer resource with a resource description and element details.
				/// Used to initialize structured or raw buffers on creation.
				/// </summary>
				/// <param name="a_sName">The debug name of the buffer resource.</param>
				/// <returns></returns>
				bool LoadByName(const std::string& a_sName);

				/// <summary>
				/// Constructs a buffer resource with a resource description and element details.
				/// Used to initialize structured or raw buffers on creation.
				/// </summary>
				/// <param name="a_sName">The debug name of the buffer resource.</param>
				/// <param name="a_ResDesc">The Direct3D resource description (e.g., buffer layout, flags).</param>
				/// <param name="a_iNumElements">The number of elements in the buffer.</param>
				/// <param name="a_iElementSize">The size in bytes of a single element.</param>
				/// <returns></returns>
				bool LoadByName(const std::string& a_sName, const D3D12_RESOURCE_DESC& a_ResDesc, size_t a_iNumElements, size_t a_iElementSize);
			protected:
				/// <summary>
				/// Creates the required views (SRV/UAV/CBV) for the buffer.
				/// Must be called after the buffer has been initialized on the GPU.
				/// </summary>
				/// <param name="a_iNumElements">The number of elements in the buffer.</param>
				/// <param name="a_iElementSize">The size in bytes of a single element.</param>
				virtual void CreateViews(size_t a_iNumElements, size_t a_iElementSize);

				BEGIN_EXPOSE_FIELDS_PARENT(Buffer, DX12Resource)
				END_EXPOSE_FIELDS(Buffer)
				BEGIN_EXPOSE_GIZMOS(Buffer)
				END_EXPOSE_GIZMOS(Buffer)
				END_EXPOSE_TO_EDITOR(Buffer)
			};
		}
	}
}