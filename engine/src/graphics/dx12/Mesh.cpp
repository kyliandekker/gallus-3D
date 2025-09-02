#include "graphics/dx12/Mesh.h"

#include <stb_image.h>
#include <filesystem>
#include <format>

// core includes
#include "core/Tool.h"
#include "core/DataStream.h"

// logger includes
#include "logger/Logger.h"

// utils includes
#include "utils/file_abstractions.h"

// graphics includes
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/CommandList.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// Mesh
			//---------------------------------------------------------------------
			Mesh::Mesh() : EngineResource()
			{}

			//---------------------------------------------------------------------
			void Mesh::Render(std::shared_ptr<CommandList> a_pCommandList, const DirectX::XMMATRIX& a_MVP)
			{
				for (MeshPartData& meshData : m_aMeshData)
				{
					a_pCommandList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					a_pCommandList->GetCommandList()->IASetVertexBuffers(0, 1, &meshData.m_VertexBuffer.GetVertexBufferView());
					a_pCommandList->GetCommandList()->IASetIndexBuffer(&meshData.m_IndexBuffer.GetIndexBufferView());

					// Update the MVP matrix
					a_pCommandList->GetCommandList()->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &a_MVP, 0);

					a_pCommandList->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(meshData.m_aIndices.size()), 1, 0, 0, 0);
				}
			}

			//---------------------------------------------------------------------
			bool Mesh::LoadByName(const std::string& a_sName)
			{
				m_sName = a_sName;

				m_ResourceType = core::ResourceType::ResourceType_Mesh;

				return true;
			}

			//---------------------------------------------------------------------
			void Mesh::SetMeshData(const MeshPartData& a_aData, const std::shared_ptr<CommandList> a_pCommandList)
			{
				if (!m_bIsDestroyable)
				{
					return;
				}

				size_t index = m_aMeshData.size();
				m_aMeshData.push_back(a_aData);
				MeshPartData& data = m_aMeshData[index];

				// Upload vertex buffer data.
				a_pCommandList->UpdateBufferResource(
					&data.m_VertexBuffer.GetResource(), &data.m_pIntermediateVertexBuffer,
					data.m_aVertices.size(), sizeof(VertexPosUV), data.m_aVertices.data());

				// Create the vertex buffer view.
				data.m_VertexBuffer.CreateViews(data.m_aVertices.size(), sizeof(VertexPosUV));

				// Upload index buffer data.
				a_pCommandList->UpdateBufferResource(
					&data.m_IndexBuffer.GetResource(), &data.m_pIntermediateIndexBuffer,
					data.m_aIndices.size(), sizeof(uint16_t), data.m_aIndices.data());

				// Create index buffer view.
				data.m_IndexBuffer.CreateViews(data.m_aIndices.size(), sizeof(uint16_t));
			}

			//---------------------------------------------------------------------
			bool Mesh::IsValid() const
			{
				return !m_aMeshData.empty();
			}
		}
	}
}