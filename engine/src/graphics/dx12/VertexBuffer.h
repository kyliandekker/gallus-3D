#pragma once

#include "graphics/dx12/Buffer.h"

// external
#include <string>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// VertexBuffer
			//---------------------------------------------------------------------
			/// <summary>
			/// A specialized buffer class for storing and managing vertex data used in rendering.
			/// Provides a vertex buffer view (VBV) that can be bound to the Direct3D input assembler stage.
			/// </summary>
			class VertexBuffer : public Buffer
			{
			public:
				/// <summary>
				/// Creates the vertex buffer view (VBV) based on the number of elements and element size.
				/// </summary>
				/// <param name="a_iNumElements">The number of vertices stored in the buffer.</param>
				/// <param name="a_iElementSize">The size in bytes of a single vertex (stride).</param>
				void CreateViews(size_t a_iNumElements, size_t a_iElementSize) override;

				/// <summary>
				/// Retrieves the Direct3D vertex buffer view used to bind the buffer to the input assembler stage.
				/// </summary>
				/// <returns>Reference to the vertex buffer view.</returns>
				D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView();

				/// <summary>
				/// Gets the number of vertices stored in the buffer.
				/// </summary>
				/// <returns>The number of vertices.</returns>
				size_t GetNumVertices() const;

				/// <summary>
				/// Gets the stride (size in bytes) of a single vertex in the buffer.
				/// </summary>
				/// <returns>The vertex stride in bytes.</returns>
				size_t GetVertexStride() const;
			private:
				size_t m_iNumVertices = 0;
				size_t m_iVertexStride = 0;

				D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

				BEGIN_EXPOSE_FIELDS_PARENT(VertexBuffer, Buffer)
				END_EXPOSE_FIELDS(VertexBuffer)
				BEGIN_EXPOSE_GIZMOS(VertexBuffer)
				END_EXPOSE_GIZMOS(VertexBuffer)
				END_EXPOSE_TO_EDITOR(VertexBuffer)
			};
		}
	}
}