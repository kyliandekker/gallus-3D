#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

#include "utils/file_abstractions.h"
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
			struct MeshPartData
			{
				std::vector<VertexPosUV> m_aVertices;
				std::vector<uint16_t> m_aIndices;

				VertexBuffer m_VertexBuffer;
				IndexBuffer m_IndexBuffer;

				Microsoft::WRL::ComPtr<ID3D12Resource> m_pIntermediateVertexBuffer;
				Microsoft::WRL::ComPtr<ID3D12Resource> m_pIntermediateIndexBuffer;
			};

			class CommandList;

			//---------------------------------------------------------------------
			// Mesh
			//---------------------------------------------------------------------
			/// <summary>
			/// Represents a renderable mesh resource in the engine.
			/// Contains vertex and index buffers, transformation data, and supports rendering with a command list.
			/// </summary>
			class Mesh : public core::EngineResource
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
				/// <param name="a_Transform">The world transform of the mesh.</param>
				/// <param name="a_CameraView">The camera view matrix.</param>
				/// <param name="a_CameraProjection">The camera projection matrix.</param>
				void Render(std::shared_ptr<CommandList> a_pCommandList, const DX12Transform& a_Transform, const DirectX::XMMATRIX& a_CameraView, const DirectX::XMMATRIX& a_CameraProjection);

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
				bool LoadByName(const std::string& a_sName, const std::shared_ptr<CommandList> a_pCommandList);
			private:
				std::vector<MeshPartData*> m_aMeshData;
			};
		}
	}
}