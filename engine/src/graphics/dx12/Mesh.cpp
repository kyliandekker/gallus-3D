#include "graphics/dx12/Mesh.h"

#include <stb_image.h>
#include <filesystem>
#include <format>
#include <tiny_gltf/tiny_gltf.h>

// core includes
#include "core/Engine.h"
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

				m_AssetType = resources::AssetType::Mesh;

				return true;
			}

			bool Mesh::GetMeshDataFromModel(const std::string& a_sName, std::vector<MeshPartData>& a_aMeshData)
			{
				resources::FileResource* fileResource = nullptr;
				if (!core::ENGINE->GetResourceAtlas().GetResource(a_sName, fileResource))
				{
					return false;
				}

				core::Data data;
				if (!fileResource->GetFileData(data))
				{
					return false;
				}


				tinygltf::Model model;
				tinygltf::TinyGLTF loader;
				std::string err, warn;
				if (!loader.LoadBinaryFromMemory(&model, &err, &warn, data.dataAs<unsigned char>(), static_cast<unsigned int>(data.size()), ""))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading bin file %s.", err.c_str());
					return false;
				}

				a_aMeshData.clear();
				a_aMeshData.reserve(model.meshes.size());
				for (const auto& mesh : model.meshes)
				{
					MeshPartData meshData;

					std::string name = std::format("{0}_{1}", m_sName, mesh.name);
					meshData.m_VertexBuffer = VertexBuffer("V_" + name);
					meshData.m_IndexBuffer = IndexBuffer("I_" + name);

					size_t indexSize = 0;
					for (const auto& primitive : mesh.primitives)
					{
						const auto& indexAccessor = model.accessors[primitive.indices];
						const auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];
						const auto& indexBuffer = model.buffers[indexBufferView.buffer];

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

						const auto& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
						const auto& posBufferView = model.bufferViews[posAccessor.bufferView];
						const auto& posBuffer = model.buffers[posBufferView.buffer];

						const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

						const float* normals = nullptr;
						if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
						{
							const auto& normalAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
							const auto& normalBufferView = model.bufferViews[normalAccessor.bufferView];
							const auto& normalBuffer = model.buffers[normalBufferView.buffer];
							normals = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
						}

						const float* colors = nullptr;
						if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
						{
							const auto& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
							const auto& colorBufferView = model.bufferViews[colorAccessor.bufferView];
							const auto& colorBuffer = model.buffers[colorBufferView.buffer];
							colors = reinterpret_cast<const float*>(&colorBuffer.data[colorBufferView.byteOffset + colorAccessor.byteOffset]);
						}

						const float* uvs = nullptr;
						if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
						{
							const auto& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
							const auto& uvBufferView = model.bufferViews[uvAccessor.bufferView];
							const auto& uvBuffer = model.buffers[uvBufferView.buffer];
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
					a_aMeshData.push_back(meshData);
				}
			}

			bool Mesh::GetMeshDataFromModel(const fs::path& a_Path, std::vector<MeshPartData>& a_aMeshData)
			{
				core::Data data;
				if (!file::LoadFile(a_Path, data))
				{
					return false;
				}

				tinygltf::Model model;
				tinygltf::TinyGLTF loader;
				std::string err, warn;
				if (!loader.LoadBinaryFromMemory(&model, &err, &warn, data.dataAs<unsigned char>(), static_cast<unsigned int>(data.size()), ""))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading bin file %s.", err.c_str());
					return false;
				}

				a_aMeshData.clear();
				a_aMeshData.reserve(model.meshes.size());
				for (const auto& mesh : model.meshes)
				{
					MeshPartData meshData;

					std::string name = std::format("{0}_{1}", m_sName, mesh.name);
					meshData.m_VertexBuffer = VertexBuffer("V_" + name);
					meshData.m_IndexBuffer = IndexBuffer("I_" + name);

					size_t indexSize = 0;
					for (const auto& primitive : mesh.primitives)
					{
						const auto& indexAccessor = model.accessors[primitive.indices];
						const auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];
						const auto& indexBuffer = model.buffers[indexBufferView.buffer];

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

						const auto& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
						const auto& posBufferView = model.bufferViews[posAccessor.bufferView];
						const auto& posBuffer = model.buffers[posBufferView.buffer];

						const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

						const float* normals = nullptr;
						if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
						{
							const auto& normalAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
							const auto& normalBufferView = model.bufferViews[normalAccessor.bufferView];
							const auto& normalBuffer = model.buffers[normalBufferView.buffer];
							normals = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
						}

						const float* colors = nullptr;
						if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
						{
							const auto& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
							const auto& colorBufferView = model.bufferViews[colorAccessor.bufferView];
							const auto& colorBuffer = model.buffers[colorBufferView.buffer];
							colors = reinterpret_cast<const float*>(&colorBuffer.data[colorBufferView.byteOffset + colorAccessor.byteOffset]);
						}

						const float* uvs = nullptr;
						if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
						{
							const auto& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
							const auto& uvBufferView = model.bufferViews[uvAccessor.bufferView];
							const auto& uvBuffer = model.buffers[uvBufferView.buffer];
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
					a_aMeshData.push_back(meshData);
				}
			}

			//---------------------------------------------------------------------
			void Mesh::SetMeshData(const MeshPartData& a_aData)
			{
				if (!m_bIsDestroyable)
				{
					return;
				}

				size_t index = m_aMeshData.size();
				m_aMeshData.push_back(a_aData);
				MeshPartData& data = m_aMeshData[index];
			}

			//---------------------------------------------------------------------
			void Mesh::SetMeshData(const std::vector<MeshPartData>& a_aData)
			{
				if (!m_bIsDestroyable)
				{
					return;
				}

				m_aMeshData = a_aData;

				m_bUploadedMeshData = false;
			}

			//---------------------------------------------------------------------
			void Mesh::UploadMesh(const std::shared_ptr<CommandList> a_pCommandList)
			{
				if (m_bUploadedMeshData)
				{
					return;
				}

				for (auto& data : m_aMeshData)
				{
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

					m_bUploadedMeshData = true;
				}
			}

			//---------------------------------------------------------------------
			bool Mesh::IsValid() const
			{
				return !m_aMeshData.empty() && m_bUploadedMeshData;
			}
		}
	}
}