#include "IndexBuffer.h"

// external
#include <cassert>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// IndexBuffer
			//---------------------------------------------------------------------
			void IndexBuffer::CreateViews(size_t a_iNumElements, size_t a_iElementSize)
			{
				assert(a_iElementSize == 2 || a_iElementSize == 4 && "Indices must be 16, or 32-bit integers.");

				m_iNumIndicies = a_iNumElements;
				m_IndexFormat = (a_iElementSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

				m_IndexBufferView.BufferLocation = m_pResource->GetGPUVirtualAddress();
				m_IndexBufferView.SizeInBytes = static_cast<UINT>(a_iNumElements * a_iElementSize);
				m_IndexBufferView.Format = m_IndexFormat;
			}

			//---------------------------------------------------------------------
			D3D12_INDEX_BUFFER_VIEW& IndexBuffer::GetIndexBufferView()
			{
				return m_IndexBufferView;
			}

			//---------------------------------------------------------------------
			size_t IndexBuffer::GetNumIndicies() const
			{
				return m_iNumIndicies;
			}

			//---------------------------------------------------------------------
			DXGI_FORMAT IndexBuffer::GetIndexFormat() const
			{
				return m_IndexFormat;
			}
		}
	}
}