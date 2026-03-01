#include "VertexBuffer.h"

namespace gallus::graphics::dx12
{
	//---------------------------------------------------------------------
	// VertexBuffer
	//---------------------------------------------------------------------
	void VertexBuffer::CreateViews(size_t a_iNumElements, size_t a_iElementSize)
	{
		m_iNumVertices = a_iNumElements;
		m_iVertexStride = a_iElementSize;

		m_VertexBufferView.BufferLocation = m_pResource->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = static_cast<UINT>(m_iNumVertices * m_iVertexStride);
		m_VertexBufferView.StrideInBytes = static_cast<UINT>(m_iVertexStride);
	}

	//---------------------------------------------------------------------
	D3D12_VERTEX_BUFFER_VIEW& VertexBuffer::GetVertexBufferView()
	{
		return m_VertexBufferView;
	}

	//---------------------------------------------------------------------
	size_t VertexBuffer::GetNumVertices() const
	{
		return m_iNumVertices;
	}

	//---------------------------------------------------------------------
	size_t VertexBuffer::GetVertexStride() const
	{
		return m_iVertexStride;
	}
}