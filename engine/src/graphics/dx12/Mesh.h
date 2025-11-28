#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

// utils includes
#include "utils/file_abstractions.h"

// graphics includes
#include "graphics/dx12/DX12Transform.h"
#include "graphics/dx12/IndexBuffer.h"
#include "graphics/dx12/VertexBuffer.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// VertexPosUV
			//---------------------------------------------------------------------
			/// <summary>
			/// A simple vertex format containing a 2D position and texture coordinates (UV).
			/// </summary>
			struct VertexPosUV
			{
				DirectX::XMFLOAT2 Position;
				DirectX::XMFLOAT2 UV;
			};

			//---------------------------------------------------------------------
			// MeshPartData
			//---------------------------------------------------------------------
			/// <summary>
			/// Represents a part of a mesh, containing vertices, indices, and GPU buffers.
			/// Useful for meshes that are split into sub-meshes or materials.
			/// </summary>
			class MeshPartData
			{
			public:
				MeshPartData(
					std::vector<VertexPosUV> a_aVertices,
					std::vector<uint16_t> a_aIndices) :
					m_aVertices(a_aVertices), m_aIndices(a_aIndices)
				{}

				std::vector<VertexPosUV> m_aVertices;
				std::vector<uint16_t> m_aIndices;

				VertexBuffer m_VertexBuffer;
				IndexBuffer m_IndexBuffer;

				Microsoft::WRL::ComPtr<ID3D12Resource> m_pIntermediateVertexBuffer;
				Microsoft::WRL::ComPtr<ID3D12Resource> m_pIntermediateIndexBuffer;
			};

			enum class PRIMITIVES
			{
				SQUARE,
			};
			inline std::vector<MeshPartData> s_PRIMITIVES = {
				MeshPartData(
					{
						{ DirectX::XMFLOAT2(-0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 0.0f) },
						{ DirectX::XMFLOAT2(0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 0.0f) },
						{ DirectX::XMFLOAT2(-0.5f, 0.5f), DirectX::XMFLOAT2(0.0f, 1.0f) },
						{ DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 1.0f) }
					},
					{
						0, 1, 2,
						2, 1, 3
					}
				)
			};

			class CommandList;
			class CommandQueue;

			//---------------------------------------------------------------------
			// Mesh
			//---------------------------------------------------------------------
			/// <summary>
			/// Represents a renderable mesh resource in the engine.
			/// Contains vertex and index buffers, transformation data, and supports rendering with a command list.
			/// </summary>
			class Mesh : public resources::EngineResource
			{
			public:
				/// <summary>
				/// Constructs an empty mesh.
				/// </summary>
				Mesh();

				/// <summary>
				/// Renders the mesh using the specified command list, applying a transform and camera matrices.
				/// </summary>
				/// <param name="a_pCommandList">The command list used to issue GPU draw calls.</param>
				/// <param name="a_MVP">The mvp of the mesh.</param>
				void Render(std::shared_ptr<CommandList> a_pCommandList, const DirectX::XMMATRIX& a_MVP);

				/// <summary>
				/// Checks whether the mesh contains valid data and is ready for rendering.
				/// </summary>
				/// <returns>True if the mesh is valid, false otherwise.</returns>
				bool IsValid() const override;

				/// <summary>
				/// Loads a mesh by its name, typically from a file or resource database, and initializes GPU buffers.
				/// </summary>
				/// <param name="a_sName">The name of the mesh to load.</param>
				/// <param name="a_pCommandList">The command list used for uploading resources to the GPU.</param>
				/// <returns>True if loading was successful, false otherwise.</returns>
				bool LoadByName(const std::string& a_sName);

				/// <summary>
				/// Sets the mesh data.
				/// </summary>
				/// <param name="a_aData">The mesh part data.</param>
				/// <param name="a_pCommandQueue">The command queue used for uploading.</param>
				void SetMeshData(const MeshPartData& a_aData, const std::shared_ptr<CommandQueue> a_pCommandQueue);
			private:
				std::vector<MeshPartData> m_aMeshData;

#ifdef _EDITOR
				BEGIN_EXPOSE_FIELDS_PARENT(Mesh, resources::EngineResource)
				END_EXPOSE_FIELDS(Mesh)
				BEGIN_EXPOSE_GIZMOS(Mesh)
				END_EXPOSE_GIZMOS(Mesh)
				END_EXPOSE_TO_EDITOR(Mesh)
#endif
			};
		}
	}
}