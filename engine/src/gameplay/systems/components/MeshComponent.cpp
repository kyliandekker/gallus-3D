#include "MeshComponent.h"

// external
#include <rapidjson/utils.h>

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/DX12ShaderBind.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12Transform2D.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/systems/TransformSystem.h"

#define JSON_MESH_COMPONENT_TEX_VAR "texture"
#define JSON_MESH_COMPONENT_TEX_NAME_VAR "name"
#define JSON_MESH_COMPONENT_TEX_TEXTURE_INDEX_VAR "textureIndex"
#define JSON_MESH_COMPONENT_MESH_VAR "mesh"
#define JSON_MESH_COMPONENT_SHADER_VAR "shader"
#define JSON_MESH_COMPONENT_SHADER_PIXEL_VAR "pixel"
#define JSON_MESH_COMPONENT_SHADER_VERTEX_VAR "vertex"
#define JSON_MESH_COMPONENT_MATERIAL_VAR "material"
#define JSON_MESH_COMPONENT_COLOR_VAR "color"
#define JSON_MESH_COMPONENT_COLOR_R_VAR "r"
#define JSON_MESH_COMPONENT_COLOR_G_VAR "g"
#define JSON_MESH_COMPONENT_COLOR_B_VAR "b"
#define JSON_MESH_COMPONENT_COLOR_A_VAR "a"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// MeshComponent
		//---------------------------------------------------------------------
		void MeshComponent::Init(const gameplay::EntityID& a_EntityID)
		{
			Component::Init(a_EntityID);

			m_pShaderBind = core::ENGINE->GetResourceAtlas().GetDefaultShaderBind();
			m_pTexture = core::ENGINE->GetResourceAtlas().GetDefaultTexture();
			m_pMesh = {};
			m_vColor = { 1, 1, 1, 1 };
		}

		//---------------------------------------------------------------------
		void MeshComponent::SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh)
		{
			m_pMesh = a_pMesh;
		}

		//---------------------------------------------------------------------
		void MeshComponent::SetShader(std::weak_ptr<graphics::dx12::DX12ShaderBind> a_pShaderBind)
		{
			m_pShaderBind = a_pShaderBind;
		}

		//---------------------------------------------------------------------
		void MeshComponent::SetTexture(std::weak_ptr<graphics::dx12::Texture> a_pTexture)
		{
			m_pTexture = a_pTexture;
		}

		//---------------------------------------------------------------------
		void MeshComponent::Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera)
		{
			const DirectX::XMMATRIX viewMatrix = a_Camera.GetViewMatrix();
			const DirectX::XMMATRIX& projectionMatrix = a_Camera.GetProjectionMatrix();

			graphics::dx12::DX12Transform2D transform;
			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
			if (transformSys.HasComponent(a_EntityID))
			{
				transform = transformSys.GetComponent(a_EntityID).Transform();
			}
			DirectX::XMMATRIX mvpMatrix = transform.GetWorldMatrixWithPivot() * viewMatrix * projectionMatrix;

			// TODO: Culling
			//if (!CheckVisibility(transform, a_Camera))
			//{
			//	return;
			//}

			if (!core::ENGINE->GetECS().GetEntity(a_EntityID))
			{
				return;
			}

			if (!core::ENGINE->GetECS().GetEntity(a_EntityID)->IsActive())
			{
				return;
			}

			float colorData[4] = { m_vColor.x, m_vColor.y, m_vColor.z, m_vColor.w };
			a_pCommandList->GetCommandList()->SetGraphicsRoot32BitConstants(graphics::dx12::RootParameters::SPRITE_COLOR, 4, colorData, 0);

			if (auto tex = m_pTexture.lock())
			{
				if (tex->CanBeDrawn())
				{
					tex->Bind(a_pCommandList, m_iTextureIndex);
				}
			}

			if (auto shaderBind = m_pShaderBind.lock())
			{
				if (shaderBind->IsValid())
				{
					shaderBind->Bind(a_pCommandList);
				}
			}

			if (auto mesh = m_pMesh.lock())
			{
				if (mesh->IsValid())
				{
					mesh->Render(a_pCommandList, mvpMatrix);
				}
			}
		}

		//---------------------------------------------------------------------
#ifdef _EDITOR
		void MeshComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			{
				if (auto tex = m_pTexture.lock())
				{
					a_Document.AddMember(JSON_MESH_COMPONENT_TEX_VAR, rapidjson::Value().SetObject(), a_Allocator);
					std::string texName = tex->GetName();
					a_Document[JSON_MESH_COMPONENT_TEX_VAR].AddMember(
						JSON_MESH_COMPONENT_TEX_NAME_VAR,
						rapidjson::Value(texName.c_str(), a_Allocator),
						a_Allocator
					);
				}
				a_Document[JSON_MESH_COMPONENT_TEX_VAR].AddMember(
					JSON_MESH_COMPONENT_TEX_TEXTURE_INDEX_VAR,
					m_iTextureIndex,
					a_Allocator
				);
			}

			a_Document.AddMember(JSON_MESH_COMPONENT_SHADER_VAR, rapidjson::Value().SetObject(), a_Allocator);

			if (auto shaderBind = m_pShaderBind.lock())
			{
				if (auto pixelShader = shaderBind->GetPixelShader().lock())
				{
					std::string pixelShaderName = pixelShader->GetName();
					a_Document[JSON_MESH_COMPONENT_SHADER_VAR].AddMember(
						JSON_MESH_COMPONENT_SHADER_PIXEL_VAR,
						rapidjson::Value(pixelShaderName.c_str(), a_Allocator),
						a_Allocator
					);
				}

				if (auto vertexShader = shaderBind->GetVertexShader().lock())
				{
					std::string vertexShaderName = vertexShader->GetName();
					a_Document[JSON_MESH_COMPONENT_SHADER_VAR].AddMember(
						JSON_MESH_COMPONENT_SHADER_VERTEX_VAR,
						rapidjson::Value(vertexShaderName.c_str(), a_Allocator),
						a_Allocator
					);
				}
			}

			if (auto mesh = m_pMesh.lock())
			{
				std::string meshName = mesh->GetName();
				a_Document.AddMember(
					JSON_MESH_COMPONENT_MESH_VAR,
					rapidjson::Value(meshName.c_str(), a_Allocator),
					a_Allocator
				);
			}

			{
				rapidjson::Document colorDoc;
				colorDoc.SetObject();

				colorDoc.AddMember(JSON_MESH_COMPONENT_COLOR_R_VAR, m_vColor.x, a_Allocator);
				colorDoc.AddMember(JSON_MESH_COMPONENT_COLOR_G_VAR, m_vColor.y, a_Allocator);
				colorDoc.AddMember(JSON_MESH_COMPONENT_COLOR_B_VAR, m_vColor.z, a_Allocator);
				colorDoc.AddMember(JSON_MESH_COMPONENT_COLOR_A_VAR, m_vColor.w, a_Allocator);

				a_Document.AddMember(
					JSON_MESH_COMPONENT_COLOR_VAR,
					colorDoc,
					a_Allocator
				);
			}
		}
#endif

		//---------------------------------------------------------------------
		void MeshComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			std::string tex = a_SrcData.GetSrc(JSON_MESH_COMPONENT_TEX_VAR).GetString(JSON_MESH_COMPONENT_TEX_NAME_VAR);
			SetTextureIndex(a_SrcData.GetSrc(JSON_MESH_COMPONENT_TEX_VAR).GetInt(JSON_MESH_COMPONENT_TEX_TEXTURE_INDEX_VAR));
			std::string pixelShader = a_SrcData.GetSrc(JSON_MESH_COMPONENT_SHADER_VAR).GetString(JSON_MESH_COMPONENT_SHADER_PIXEL_VAR);
			std::string vertexShader = a_SrcData.GetSrc(JSON_MESH_COMPONENT_SHADER_VAR).GetString(JSON_MESH_COMPONENT_SHADER_VERTEX_VAR);
			std::string mesh = a_SrcData.GetString(JSON_MESH_COMPONENT_MESH_VAR);

			std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = core::ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
			if (!mesh.empty())
			{
				SetMesh(core::ENGINE->GetResourceAtlas().LoadMesh(mesh));
			}
			if (!tex.empty())
			{
				SetTexture(core::ENGINE->GetResourceAtlas().LoadTexture(tex, cCommandQueue));
			}
			if (!vertexShader.empty() && !pixelShader.empty())
			{
				SetShader(core::ENGINE->GetResourceAtlas().LoadShaderBind(
					pixelShader,
					core::ENGINE->GetResourceAtlas().LoadPixelShader(pixelShader),
					core::ENGINE->GetResourceAtlas().LoadVertexShader(vertexShader)
				));
			}
			cCommandQueue->Flush();
		}

		/// <summary>
		/// Sets the sprite index.
		/// </summary>
		/// <param name="a_iMeshIndex">The index the sprite should have.</param>
		void MeshComponent::SetTextureIndex(int8_t a_iMeshIndex)
		{
			size_t numMeshRects = 0;
			if (auto tex = m_pTexture.lock())
			{
				numMeshRects = tex->GetSpriteRectsSize() - 1;
			}
			if (a_iMeshIndex < 0)
			{
				a_iMeshIndex = 0;
			}
			else if (a_iMeshIndex > numMeshRects)
			{
				a_iMeshIndex = numMeshRects;
			}
			m_iTextureIndex = a_iMeshIndex;
		}
	}
}