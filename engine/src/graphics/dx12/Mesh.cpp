#include "graphics/dx12/Mesh.h"

// external
#include <tiny_gltf/tiny_gltf.h>
#include <format>

// core
#include "core/Data.h"
#include "core/Engine.h"

// graphics
#include "graphics/dx12/DX12UploadBufferAllocator.h"
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/ShaderDefs.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

// logger
#include "logger/Logger.h"

namespace gallus::graphics::dx12
{
	//---------------------------------------------------------------------
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

	//---------------------------------------------------------------------
	MeshPartData CreateCube()
	{
		return {
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
		};
	}

	//---------------------------------------------------------------------
	MeshPartData CreateSprite()
	{
		return
			MeshPartData(
				{
					{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
				{ DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }
				},
				{ 0, 1, 2, 2, 1, 3 }
			);
	}

	//---------------------------------------------------------------------
	std::vector<MeshPartData> s_PRIMITIVES = {
		CreateSprite(),
		CreateCube(),
		CreateCylinder()
	};

	//---------------------------------------------------------------------
	// Mesh
	//---------------------------------------------------------------------
	Mesh::~Mesh()
	{}

	//---------------------------------------------------------------------
	bool Mesh::LoadData(const core::Data& a_Data, std::shared_ptr<CommandQueue> a_pCommandQueue)
	{
		m_AssetType = resources::AssetType::Mesh;

		std::vector<MeshPartData> outData;
		if (!GetMeshDataFromModel(a_Data, outData))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to load mesh \"%s\": Something wrong with mesh data.", m_sName.c_str());
			return false;
		}
		
		SetMeshData(outData, a_pCommandQueue);
		UploadMeshData(a_pCommandQueue);

		LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully loaded mesh \"%s\".", m_sName.c_str());
		return true;
	}

	//---------------------------------------------------------------------
	bool Mesh::LoadData(const MeshPartData& a_Data, std::shared_ptr<CommandQueue> a_pCommandQueue)
	{
		m_AssetType = resources::AssetType::Mesh;

		SetMeshData({ a_Data }, a_pCommandQueue);
		UploadMeshData(a_pCommandQueue);

		LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully loaded mesh \"%s\".", m_sName.c_str());
		return true;
	}

	//---------------------------------------------------------------------
	bool Mesh::GetMeshDataFromModel(const core::Data& a_Data, std::vector<MeshPartData>& a_aOutData)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err, warn;

		if (!loader.LoadBinaryFromMemory(
			&model, &err, &warn,
			a_Data.dataAs<unsigned char>(),
			static_cast<unsigned int>(a_Data.size()), ""))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12,
				"Failed loading mesh data: %s.", err.c_str());
			return false;
		}

		a_aOutData.clear();

		//--------------------------------------------------
		// LOAD MESHES
		//--------------------------------------------------

		a_aOutData.reserve(model.meshes.size());

		for (const tinygltf::Mesh& mesh : model.meshes)
		{
			MeshPartData meshData;

			std::string name = std::format("{0}_{1}", m_sName, mesh.name);
			meshData.m_VertexBuffer.SetName("V_" + name);
			meshData.m_IndexBuffer.SetName("I_" + name);

			for (const tinygltf::Primitive& primitive : mesh.primitives)
			{
				//------------------------------------------
				// POSITION
				//------------------------------------------

				const tinygltf::Accessor& posAccessor =
					model.accessors[primitive.attributes.find("POSITION")->second];

				const tinygltf::BufferView& posBufferView =
					model.bufferViews[posAccessor.bufferView];

				const tinygltf::Buffer& posBuffer =
					model.buffers[posBufferView.buffer];

				const float* positions =
					reinterpret_cast<const float*>(
						&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

				//------------------------------------------
				// NORMAL
				//------------------------------------------

				const float* normals = nullptr;

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
				{
					const tinygltf::Accessor& normalAccessor =
						model.accessors[primitive.attributes.find("NORMAL")->second];

					const tinygltf::BufferView& normalBufferView =
						model.bufferViews[normalAccessor.bufferView];

					const tinygltf::Buffer& normalBuffer =
						model.buffers[normalBufferView.buffer];

					normals = reinterpret_cast<const float*>(
						&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
				}

				//------------------------------------------
				// UV
				//------------------------------------------

				const float* uvs = nullptr;

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& uvAccessor =
						model.accessors[primitive.attributes.find("TEXCOORD_0")->second];

					const tinygltf::BufferView& uvBufferView =
						model.bufferViews[uvAccessor.bufferView];

					const tinygltf::Buffer& uvBuffer =
						model.buffers[uvBufferView.buffer];

					uvs = reinterpret_cast<const float*>(
						&uvBuffer.data[uvBufferView.byteOffset + uvAccessor.byteOffset]);
				}

				//------------------------------------------
				// JOINTS (bone indices)
				//------------------------------------------

				const uint8_t* jointsU8 = nullptr;
				const uint16_t* jointsU16 = nullptr;

				if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& jointAccessor =
						model.accessors[primitive.attributes.find("JOINTS_0")->second];

					const tinygltf::BufferView& jointBufferView =
						model.bufferViews[jointAccessor.bufferView];

					const tinygltf::Buffer& jointBuffer =
						model.buffers[jointBufferView.buffer];

					if (jointAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
					{
						jointsU8 = reinterpret_cast<const uint8_t*>(
							&jointBuffer.data[jointBufferView.byteOffset + jointAccessor.byteOffset]);
					}
					else if (jointAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						jointsU16 = reinterpret_cast<const uint16_t*>(
							&jointBuffer.data[jointBufferView.byteOffset + jointAccessor.byteOffset]);
					}
				}

				//------------------------------------------
				// WEIGHTS (bone weights)
				//------------------------------------------

				const float* weightsF32 = nullptr;
				const uint8_t* weightsU8 = nullptr;

				if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& weightAccessor =
						model.accessors[primitive.attributes.find("WEIGHTS_0")->second];

					const tinygltf::BufferView& weightBufferView =
						model.bufferViews[weightAccessor.bufferView];

					const tinygltf::Buffer& weightBuffer =
						model.buffers[weightBufferView.buffer];

					if (weightAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
					{
						weightsF32 = reinterpret_cast<const float*>(
							&weightBuffer.data[weightBufferView.byteOffset + weightAccessor.byteOffset]);
					}
					else if (weightAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
					{
						weightsU8 = reinterpret_cast<const uint8_t*>(
							&weightBuffer.data[weightBufferView.byteOffset + weightAccessor.byteOffset]);
					}
				}

				//------------------------------------------
				// INDICES
				//------------------------------------------

				const tinygltf::Accessor& indexAccessor =
					model.accessors[primitive.indices];

				const tinygltf::BufferView& indexBufferView =
					model.bufferViews[indexAccessor.bufferView];

				const tinygltf::Buffer& indexBuffer =
					model.buffers[indexBufferView.buffer];

				const uint8_t* indices =
					&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset];

				meshData.m_aIndices.resize(indexAccessor.count);
				if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
				{
					const uint16_t* src = reinterpret_cast<const uint16_t*>(indices);
					memcpy(meshData.m_aIndices.data(), src, indexAccessor.count * sizeof(uint16_t));
				}
				else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				{
					const uint32_t* src = reinterpret_cast<const uint32_t*>(indices);
					for (size_t i = 0; i < indexAccessor.count; ++i)
					{
						meshData.m_aIndices[i] = static_cast<uint16_t>(src[i]);
					}
				}

				//------------------------------------------
				// VERTICES
				//------------------------------------------

				for (size_t i = 0; i < posAccessor.count; ++i)
				{
					VSInput v;

					v.POSITION = DirectX::XMFLOAT3(
						positions[i * 3],
						positions[i * 3 + 1],
						positions[i * 3 + 2]);

					v.NORMAL = normals ?
						DirectX::XMFLOAT3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]) :
						DirectX::XMFLOAT3(0, 0, 1);

					v.TEXCOORD = uvs ?
						DirectX::XMFLOAT2(uvs[i * 2], uvs[i * 2 + 1]) :
						DirectX::XMFLOAT2(0, 0);

					if ((jointsU8 || jointsU16) && (weightsF32 || weightsU8))
					{
						if (jointsU16)
						{
							v.JOINTS.x = jointsU16[i * 4];
							v.JOINTS.y = jointsU16[i * 4 + 1];
							v.JOINTS.z = jointsU16[i * 4 + 2];
							v.JOINTS.w = jointsU16[i * 4 + 3];
						}
						else if (jointsU8)
						{
							v.JOINTS.x = jointsU8[i * 4];
							v.JOINTS.y = jointsU8[i * 4 + 1];
							v.JOINTS.z = jointsU8[i * 4 + 2];
							v.JOINTS.w = jointsU8[i * 4 + 3];
						}

						if (weightsF32)
						{
							v.WEIGHTS.x = weightsF32[i * 4];
							v.WEIGHTS.y = weightsF32[i * 4 + 1];
							v.WEIGHTS.z = weightsF32[i * 4 + 2];
							v.WEIGHTS.w = weightsF32[i * 4 + 3];
						}
						else if (weightsU8)
						{
							v.WEIGHTS.x = weightsU8[i * 4] / 255.0f;
							v.WEIGHTS.y = weightsU8[i * 4 + 1] / 255.0f;
							v.WEIGHTS.z = weightsU8[i * 4 + 2] / 255.0f;
							v.WEIGHTS.w = weightsU8[i * 4 + 3] / 255.0f;
						}
					}
					else
					{
						for (int j = 0; j < 4; j++)
						{
							v.JOINTS.x = 0;
							v.JOINTS.y = 0;
							v.JOINTS.z = 0;
							v.JOINTS.w = 0;

							v.WEIGHTS.x = (j == 0) ? 1.0f : 0.0f;
							v.WEIGHTS.y = (j == 0) ? 1.0f : 0.0f;
							v.WEIGHTS.z = (j == 0) ? 1.0f : 0.0f;
							v.WEIGHTS.w = (j == 0) ? 1.0f : 0.0f;
						}
					}

					meshData.m_aVertices.push_back(v);
				}
			}

			a_aOutData.push_back(meshData);
		}

		//--------------------------------------------------
		// LOAD BONE INFO
		//--------------------------------------------------
		m_aBoneInfo.clear();

		for (const tinygltf::Skin& skin : model.skins)
		{
			for (size_t jointIdx = 0; jointIdx < skin.joints.size(); ++jointIdx)
			{
				int nodeIdx = skin.joints[jointIdx];
				if (nodeIdx < 0 || nodeIdx >= (int) model.nodes.size())
				{
					continue;
				}

				const tinygltf::Node& node = model.nodes[nodeIdx];

				// Use node name, or fallback to Bone_0, Bone_1...
				std::string boneName = node.name.empty() ? std::format("Bone_{}", jointIdx) : node.name;
				m_aBoneInfo.push_back(boneName);
			}
		}

		return true;
	}

	//---------------------------------------------------------------------
	void Mesh::SetMeshData(const std::vector<MeshPartData>& a_aData, const std::shared_ptr<CommandQueue> a_pCommandQueue)
	{
		if (!m_bIsDestroyable)
		{
			return;
		}

		m_aMeshData.clear();
		m_aMeshData = a_aData;

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
				meshData.m_aVertices.size(), sizeof(VSInput), meshData.m_aVertices.data());

			// Create the vertex buffer view.
			meshData.m_VertexBuffer.CreateViews(meshData.m_aVertices.size(), sizeof(VSInput));

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
	void Mesh::RenderMeshData(MeshPartData& a_MeshData, std::shared_ptr<CommandList> a_pCommandList)
	{
		a_pCommandList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		a_pCommandList->GetCommandList()->IASetVertexBuffers(0, 1, &a_MeshData.m_VertexBuffer.GetVertexBufferView());
		a_pCommandList->GetCommandList()->IASetIndexBuffer(&a_MeshData.m_IndexBuffer.GetIndexBufferView());

		a_pCommandList->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(a_MeshData.m_aIndices.size()), 1, 0, 0, 0);
	}

	//---------------------------------------------------------------------
	bool Mesh::IsValid() const
	{
		return !m_aMeshData.empty();
	}
}