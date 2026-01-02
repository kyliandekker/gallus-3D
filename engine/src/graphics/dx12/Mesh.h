#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

// external
#include <string>
#include <vector>
#include <cstdint>
#include <memory>

// graphics
#include "graphics/dx12/Transform.h"
#include "graphics/dx12/IndexBuffer.h"
#include "graphics/dx12/VertexBuffer.h"

// utils
#include "utils/file_abstractions.h"

namespace gallus
{
	namespace core
	{
		class Data;
	}
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
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT2 UV;
				DirectX::XMFLOAT3 Normal;
				DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
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
				MeshPartData() = default;

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
				CUBE,
				CYLINDER
			};

			inline MeshPartData CreateCylinder()
			{
				MeshPartData mesh;
				int SEGMENTS = 6;
				float RADIUS = 0.5f;
				float HALF_HEIGHT = 0.5f;

				for (int i = 0; i <= SEGMENTS; i++)
				{
					float t = static_cast<float>(i) / static_cast<float>(SEGMENTS);
					float angle = t * DirectX::XM_2PI;

					float x = cosf(angle) * RADIUS;
					float z = sinf(angle) * RADIUS;

					DirectX::XMFLOAT3 normal(x / RADIUS, 0.0f, z / RADIUS);

					mesh.m_aVertices.push_back(
						{
							DirectX::XMFLOAT3(x, -HALF_HEIGHT, z),
							DirectX::XMFLOAT2(t, 1.0f),
							normal
						}
					);

					mesh.m_aVertices.push_back(
						{
							DirectX::XMFLOAT3(x, HALF_HEIGHT, z),
							DirectX::XMFLOAT2(t, 0.0f),
							normal
						}
					);
				}

				for (int i = 0; i < SEGMENTS; i++)
				{
					uint32_t baseIndex = static_cast<uint32_t>(i * 2);

					mesh.m_aIndices.push_back(baseIndex + 0);
					mesh.m_aIndices.push_back(baseIndex + 1);
					mesh.m_aIndices.push_back(baseIndex + 3);

					mesh.m_aIndices.push_back(baseIndex + 3);
					mesh.m_aIndices.push_back(baseIndex + 2);
					mesh.m_aIndices.push_back(baseIndex + 0);
				}

				uint32_t topCenterIndex = static_cast<uint32_t>(mesh.m_aVertices.size());

				mesh.m_aVertices.push_back(
					{
						DirectX::XMFLOAT3(0.0f, HALF_HEIGHT, 0.0f),
						DirectX::XMFLOAT2(0.5f, 0.5f),
						DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)
					}
				);

				for (int i = 0; i <= SEGMENTS; i++)
				{
					float t = static_cast<float>(i) / static_cast<float>(SEGMENTS);
					float angle = t * DirectX::XM_2PI;

					float x = cosf(angle) * RADIUS;
					float z = sinf(angle) * RADIUS;

					mesh.m_aVertices.push_back(
						{
							DirectX::XMFLOAT3(x, HALF_HEIGHT, z),
							DirectX::XMFLOAT2((x / RADIUS + 1.0f) * 0.5f, (z / RADIUS + 1.0f) * 0.5f),
							DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)
						}
					);
				}

				for (int i = 0; i < SEGMENTS; i++)
				{
					mesh.m_aIndices.push_back(topCenterIndex);
					mesh.m_aIndices.push_back(topCenterIndex + i + 2);
					mesh.m_aIndices.push_back(topCenterIndex + i + 1);
				}

				uint32_t bottomCenterIndex = static_cast<uint32_t>(mesh.m_aVertices.size());

				mesh.m_aVertices.push_back(
					{
						DirectX::XMFLOAT3(0.0f, -HALF_HEIGHT, 0.0f),
						DirectX::XMFLOAT2(0.5f, 0.5f),
						DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f)
					}
				);

				for (int i = 0; i <= SEGMENTS; i++)
				{
					float t = static_cast<float>(i) / static_cast<float>(SEGMENTS);
					float angle = t * DirectX::XM_2PI;

					float x = cosf(angle) * RADIUS;
					float z = sinf(angle) * RADIUS;

					mesh.m_aVertices.push_back(
						{
							DirectX::XMFLOAT3(x, -HALF_HEIGHT, z),
							DirectX::XMFLOAT2((x / RADIUS + 1.0f) * 0.5f, (z / RADIUS + 1.0f) * 0.5f),
							DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f)
						}
					);
				}

				for (int i = 0; i < SEGMENTS; i++)
				{
					mesh.m_aIndices.push_back(bottomCenterIndex);
					mesh.m_aIndices.push_back(bottomCenterIndex + i + 1);
					mesh.m_aIndices.push_back(bottomCenterIndex + i + 2);
				}

				return mesh;
			}

			inline std::vector<MeshPartData> s_PRIMITIVES = {
				MeshPartData(
					{
						{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
						{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
						{ DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
						{ DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }
					},
					{ 0, 1, 2, 2, 1, 3 }
				),
				MeshPartData(
					{
						{ DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
						{ DirectX::XMFLOAT3(0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
						{ DirectX::XMFLOAT3(0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
						{ DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },

						{ DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
						{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
						{ DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
						{ DirectX::XMFLOAT3(0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },

						{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },
						{ DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },
						{ DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },
						{ DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },

						{ DirectX::XMFLOAT3(0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
						{ DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
						{ DirectX::XMFLOAT3(0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
						{ DirectX::XMFLOAT3(0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },

						{ DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
						{ DirectX::XMFLOAT3(0.5f,  0.5f,  0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
						{ DirectX::XMFLOAT3(0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
						{ DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },

						{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
						{ DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
						{ DirectX::XMFLOAT3(0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
						{ DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) }
					},
					{
						 0,  1,  2,  2,  3,  0,
						 4,  5,  6,  6,  7,  4,
						 8,  9, 10, 10, 11,  8,
						12, 13, 14, 14, 15, 12,
						16, 17, 18, 18, 19, 16,
						20, 21, 22, 22, 23, 20
					}
				),
				CreateCylinder()
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
				Mesh() : EngineResource()
				{
					m_AssetType = resources::AssetType::Mesh;
				}

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
				/// <returns>True if loading was successful, false otherwise.</returns>
				bool LoadByName(const std::string& a_sName, std::shared_ptr<CommandQueue> a_pCommandQueue);

#ifdef _LOAD_BY_PATH
				/// <summary>
				/// Loads meshes directly from file paths.
				/// </summary>
				/// <param name="a_MeshPath">The file path to the mesh resource.</param>
				/// <returns>True if loading was successful, false otherwise.</returns>
				bool LoadByPath(const fs::path& a_MeshPath, std::shared_ptr<CommandQueue> a_pCommandQueue);
#endif // _LOAD_BY_PATH
				bool GetMeshDataFromModel(const core::Data& a_Data);

				/// <summary>
				/// Loads an empty mesh.
				/// </summary>
				/// <param name="a_sName">The name of the mesh to load.</param>
				/// <returns>True if loading was successful, false otherwise.</returns>
				bool LoadByNameEmpty(const std::string& a_sName);

				/// <summary>
				/// Sets the mesh data.
				/// </summary>
				/// <param name="a_aData">The mesh part data.</param>
				/// <param name="a_pCommandQueue">The command queue used for uploading.</param>
				void SetMeshData(const MeshPartData& a_aData, const std::shared_ptr<CommandQueue> a_pCommandQueue);

				/// <summary>
				/// Uploads the mesh data.
				/// </summary>
				/// <param name="a_pCommandQueue">The command queue used for uploading.</param>
				void UploadMeshData(const std::shared_ptr<CommandQueue> a_pCommandQueue);
			private:
				std::vector<MeshPartData> m_aMeshData;

				BEGIN_SERIALIZE_PARENT(Mesh, resources::EngineResource)
				END_SERIALIZE(Mesh)
			};
		}
	}
}