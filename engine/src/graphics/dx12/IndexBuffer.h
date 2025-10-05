#pragma once

#include "graphics/dx12/Buffer.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// IndexBuffer
			//---------------------------------------------------------------------
			/// <summary>
			/// A specialized buffer class for storing and managing index data used in indexed rendering.
			/// Provides an index buffer view (IBV) that can be bound to the Direct3D pipeline.
			/// </summary>
			class IndexBuffer : public Buffer
			{
			public:
				/// <summary>
				/// Constructs an index buffer with an optional debug name.
				/// </summary>
				/// <param name="string">The debug name of the buffer resource (default empty).</param>
				IndexBuffer(const std::string& string = "");

				/// <summary>
				/// Destroys the index buffer and releases GPU resources.
				/// </summary>
				virtual ~IndexBuffer();
								/// <summary>
				/// Creates the index buffer view (IBV) based on the number of elements and element size.
				/// </summary>
				/// <param name="a_iNumElements">The number of indices stored in the buffer.</param>
				/// <param name="a_iElementSize">The size in bytes of a single index (typically 2 for 16-bit or 4 for 32-bit).</param>
				void CreateViews(size_t a_iNumElements, size_t a_iElementSize) override;
				
				/// <summary>
				/// Retrieves the Direct3D index buffer view used to bind the buffer to the input assembler stage.
				/// </summary>
				/// <returns>Reference to the index buffer view.</returns>
				D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView();
				
				/// <summary>
				/// Gets the number of indices stored in the buffer.
				/// </summary>
				/// <returns>The number of indices.</returns>
				size_t GetNumIndicies() const;

				/// <summary>
				/// Gets the format of the indices (DXGI_FORMAT_R16_UINT or DXGI_FORMAT_R32_UINT).
				/// </summary>
				/// <returns>The DXGI format of the index buffer.</returns>
				DXGI_FORMAT GetIndexFormat() const;
			private:
				size_t m_iNumIndicies = 0;
				DXGI_FORMAT m_IndexFormat;

				D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
			};
		}
	}
}