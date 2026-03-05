#include "graphics/dx12/Mesh.h"

// external
#include <tiny_gltf/tiny_gltf.h>
#include <format>

// core
#include "core/Data.h"
#include "core/Engine.h"

// graphics
#include "graphics/dx12/ShaderDefs.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

// logger
#include "logger/Logger.h"

namespace gallus::graphics::dx12
{
	MeshPartData CreateCylinder()
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

	std::vector<MeshPartData> s_PRIMITIVES = {
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
					{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },

				{ DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
				{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
				{ DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },
				{ DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) },

				{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },
				{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },
				{ DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },
				{ DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) },

				{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
				{ DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
				{ DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
				{ DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },

				{ DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
				{ DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
				{ DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
				{ DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },

				{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
				{ DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
				{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },
				{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) }
				},
			{
				0, 1, 2, 2, 3, 0,
				4, 5, 6, 6, 7, 4,
				8, 9, 10, 10, 11, 8,
				12, 13, 14, 14, 15, 12,
				16, 17, 18, 18, 19, 16,
				20, 21, 22, 22, 23, 20
			}
			),
			CreateCylinder()
	};

	//---------------------------------------------------------------------
	// Mesh
	//---------------------------------------------------------------------
	bool Mesh::LoadData(const core::Data& a_Data)
	{
		LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Failed loading mesh: Should not use default LoadData.");
		return false;
	}

	//---------------------------------------------------------------------
	bool Mesh::LoadData(const core::Data& a_Data, std::shared_ptr<CommandQueue> a_pCommandQueue)
	{
		m_AssetType = resources::AssetType::Mesh;
		
		if (!GetMeshDataFromModel(a_Data))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to load mesh \"%s\": Something wrong with mesh data.", m_sName.c_str());
			return false;
		}
		UploadMeshData(a_pCommandQueue);

		LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully loaded mesh \"%s\".", m_sName.c_str());
		return true;
	}

	//---------------------------------------------------------------------
	void Mesh::Render(std::shared_ptr<CommandList> a_pCommandList, const DirectX::XMMATRIX& a_MVP, const DirectX::XMMATRIX& a_M)
	{
		for (MeshPartData& meshData : m_aMeshData)
		{
			a_pCommandList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			a_pCommandList->GetCommandList()->IASetVertexBuffers(0, 1, &meshData.m_VertexBuffer.GetVertexBufferView());
			a_pCommandList->GetCommandList()->IASetIndexBuffer(&meshData.m_IndexBuffer.GetIndexBufferView());

			// Update the MVP matrix
			ShaderTransform shaderTransform;
			shaderTransform.WorldViewProj = a_MVP;
			shaderTransform.WorldMatrix = a_M;

			a_pCommandList->GetCommandList()->SetGraphicsRoot32BitConstants(RootParameters::CBV, sizeof(ShaderTransform) / 4, &shaderTransform, 0);

			a_pCommandList->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(meshData.m_aIndices.size()), 1, 0, 0, 0);
		}
	}

	//---------------------------------------------------------------------
	void Mesh::SetMeshData(const MeshPartData& a_Data, const std::shared_ptr<CommandQueue> a_pCommandQueue)
	{
		if (!m_bIsDestroyable)
		{
			return;
		}

		size_t index = m_aMeshData.size();
		m_aMeshData.push_back(a_Data);

		UploadMeshData(a_pCommandQueue);
	}

	//---------------------------------------------------------------------
	void Mesh::UploadMeshData(const std::shared_ptr<CommandQueue> a_pCommandQueue)
	{
		for (MeshPartData& meshData : m_aMeshData)
		{
			std::shared_ptr<CommandList> commandList = a_pCommandQueue->GetCommandList();

			// Upload vertex buffer data.
			commandList->UpdateBufferResource(
				&meshData.m_VertexBuffer.GetResource(), &meshData.m_pIntermediateVertexBuffer,
				meshData.m_aVertices.size(), sizeof(VertexPosUV), meshData.m_aVertices.data());

			// Create the vertex buffer view.
			meshData.m_VertexBuffer.CreateViews(meshData.m_aVertices.size(), sizeof(VertexPosUV));

			// Upload index buffer data.
			commandList->UpdateBufferResource(
				&meshData.m_IndexBuffer.GetResource(), &meshData.m_pIntermediateIndexBuffer,
				meshData.m_aIndices.size(), sizeof(uint16_t), meshData.m_aIndices.data());

			// Create index buffer view.
			meshData.m_IndexBuffer.CreateViews(meshData.m_aIndices.size(), sizeof(uint16_t));

			uint64_t fenceValue = a_pCommandQueue->ExecuteCommandList(commandList);
			a_pCommandQueue->WaitForFenceValue(fenceValue);
		}
	}

	//---------------------------------------------------------------------
	bool Mesh::IsValid() const
	{
		return !m_aMeshData.empty();
	}

	//---------------------------------------------------------------------
	bool Mesh::GetMeshDataFromModel(const core::Data& a_Data)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err, warn;
		if (!loader.LoadBinaryFromMemory(&model, &err, &warn, a_Data.dataAs<unsigned char>(), static_cast<unsigned int>(a_Data.size()), ""))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading mesh data: Failed loading bin file %s.", err.c_str());
			return false;
		}

		m_aMeshData.clear();
		m_aMeshData.reserve(model.meshes.size());
		for (const tinygltf::Mesh& mesh : model.meshes)
		{
			MeshPartData meshData;

			std::string name = std::format("{0}_{1}", m_sName, mesh.name);
			meshData.m_VertexBuffer.SetName("V_" + name);
			meshData.m_IndexBuffer.SetName("I_" + name);

			size_t indexSize = 0;
			for (const tinygltf::Primitive& primitive : mesh.primitives)
			{
				const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
				const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

				// Determine index type and load data
				switch (indexAccessor.componentType)
				{
					case TINYGLTF_COMPONENT_TYPE_SHORT:
					{
						indexSize = sizeof(int16_t);
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					{
						indexSize = sizeof(uint16_t);
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_INT:
					{
						indexSize = sizeof(int32_t);
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
					{
						indexSize = sizeof(uint32_t);
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_FLOAT:
					{
						indexSize = sizeof(float);
						break;
					}
				}

				const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
				const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];

				const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

				const float* normals = nullptr;
				if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
				{
					const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
					const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
					normals = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
				}

				const float* colors = nullptr;
				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
					const tinygltf::BufferView& colorBufferView = model.bufferViews[colorAccessor.bufferView];
					const tinygltf::Buffer& colorBuffer = model.buffers[colorBufferView.buffer];
					colors = reinterpret_cast<const float*>(&colorBuffer.data[colorBufferView.byteOffset + colorAccessor.byteOffset]);
				}

				const float* uvs = nullptr;
				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& uvBufferView = model.bufferViews[uvAccessor.bufferView];
					const tinygltf::Buffer& uvBuffer = model.buffers[uvBufferView.buffer];
					uvs = reinterpret_cast<const float*>(&uvBuffer.data[uvBufferView.byteOffset + uvAccessor.byteOffset]);
				}

				for (size_t i = 0; i < posAccessor.count; ++i)
				{
					DirectX::XMFLOAT3 position(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
					DirectX::XMFLOAT3 normal = normals ? DirectX::XMFLOAT3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]) : DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f); // Default normal (Z axis)
					DirectX::XMFLOAT3 color = colors ? DirectX::XMFLOAT3(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]) : DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
					DirectX::XMFLOAT2 uv = uvs ? DirectX::XMFLOAT2(uvs[i * 2], uvs[i * 2 + 1]) : DirectX::XMFLOAT2(0.0f, 0.0f);
					meshData.m_aVertices.push_back({ position, uv, normal, color });
				}

				const uint8_t* indices = &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset];
				meshData.m_aIndices.resize(indexAccessor.count * indexSize);
				std::memcpy(meshData.m_aIndices.data(), indices, indexAccessor.count * indexSize);
			}
			m_aMeshData.push_back(meshData);
		}
		return true;
	}
}