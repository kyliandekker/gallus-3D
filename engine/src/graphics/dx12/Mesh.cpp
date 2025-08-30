#include "graphics/dx12/Mesh.h"

#include <stb_image.h>
#include <filesystem>
#include <format>

#include "core/Tool.h"
#include "utils/file_abstractions.h"
#include "core/DataStream.h"
#include "logger/Logger.h"
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
				for (MeshPartData* meshData : m_aMeshData)
				{
					a_pCommandList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					a_pCommandList->GetCommandList()->IASetVertexBuffers(0, 1, &meshData->m_VertexBuffer.GetVertexBufferView());
					a_pCommandList->GetCommandList()->IASetIndexBuffer(&meshData->m_IndexBuffer.GetIndexBufferView());

					// Update the MVP matrix
					a_pCommandList->GetCommandList()->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &a_MVP, 0);

					a_pCommandList->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(meshData->m_aIndices.size()), 1, 0, 0, 0);
				}
			}

			//---------------------------------------------------------------------
			bool Mesh::LoadByName(const std::string& a_sName, const std::shared_ptr<CommandList> a_pCommandList)
			{
				m_sName = a_sName;

				MeshPartData* meshData = new MeshPartData();
				m_aMeshData.push_back(meshData);

				meshData->m_aVertices = {
					{ DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) }, 
					{ DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }, 
					{ DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) }, 
					{ DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }  
				};

				meshData->m_aIndices = {
					0, 1, 2,
					2, 1, 3
				};

					// Upload vertex buffer data.
				a_pCommandList->UpdateBufferResource(
					&meshData->m_VertexBuffer.GetResource(), &meshData->m_pIntermediateVertexBuffer,
					meshData->m_aVertices.size(), sizeof(VertexPosUV), meshData->m_aVertices.data());

				// Create the vertex buffer view.
				meshData->m_VertexBuffer.CreateViews(meshData->m_aVertices.size(), sizeof(VertexPosUV));

				// Upload index buffer data.
				a_pCommandList->UpdateBufferResource(
					&meshData->m_IndexBuffer.GetResource(), &meshData->m_pIntermediateIndexBuffer,
					meshData->m_aIndices.size(), sizeof(uint16_t), meshData->m_aIndices.data());

				// Create index buffer view.
				meshData->m_IndexBuffer.CreateViews(meshData->m_aIndices.size(), sizeof(uint16_t));

				m_ResourceType = core::ResourceType::ResourceType_Mesh;

				return true;
			}

			//---------------------------------------------------------------------
			bool Mesh::IsValid() const
			{
				return !m_aMeshData.empty();
			}
		}
	}
}