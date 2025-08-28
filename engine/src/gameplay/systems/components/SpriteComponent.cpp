#include "gameplay/systems/components/SpriteComponent.h"

#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12Transform.h"
#include "core/Tool.h"

#include "gameplay/systems/TransformSystem.h"

#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

#define JSON_MESH_COMPONENT_TEX_VAR "texture"
#define JSON_MESH_COMPONENT_MESH_VAR "mesh"
#define JSON_MESH_COMPONENT_SHADER_VAR "shader"
#define JSON_MESH_COMPONENT_SHADER_PIXEL_VAR "pixel"
#define JSON_MESH_COMPONENT_SHADER_VERTEX_VAR "vertex"
#define JSON_MESH_COMPONENT_MATERIAL_VAR "material"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// SpriteComponent
		//---------------------------------------------------------------------
		void SpriteComponent::Init()
		{
			m_pShader = core::TOOL->GetResourceAtlas().GetDefaultShader();
			m_pTexture = core::TOOL->GetResourceAtlas().GetDefaultTexture();
			m_pMesh = core::TOOL->GetResourceAtlas().GetDefaultMesh();
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetMesh(std::shared_ptr<graphics::dx12::Mesh> a_pMesh)
		{
			m_pMesh = a_pMesh;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetShader(std::shared_ptr<graphics::dx12::Shader> a_pShader)
		{
			m_pShader = a_pShader;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetTexture(std::shared_ptr<graphics::dx12::Texture> a_pTexture)
		{
			m_pTexture = a_pTexture;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera)
		{
			const DirectX::XMMATRIX viewMatrix = a_Camera.GetViewMatrix();
			const DirectX::XMMATRIX& projectionMatrix = a_Camera.GetProjectionMatrix();

			if (!core::TOOL->GetECS().GetEntity(a_EntityID))
			{
				return;
			}

			if (!core::TOOL->GetECS().GetEntity(a_EntityID)->IsActive())
			{
				return;
			}

			if (m_pTexture && m_pTexture->CanBeDrawn())
			{
				m_pTexture->Bind(a_pCommandList);
			}

			if (m_pShader)
			{
				m_pShader->Bind(a_pCommandList);
			}

			if (m_pMesh)
			{
				graphics::dx12::DX12Transform transform;
				TransformSystem& transformSys = core::TOOL->GetECS().GetSystem<TransformSystem>();
				if (transformSys.HasComponent(a_EntityID))
				{
					transform = transformSys.GetComponent(a_EntityID).Transform();
				}
				m_pMesh->Render(a_pCommandList, transform, viewMatrix, projectionMatrix);
			}

			if (m_pTexture && m_pTexture->IsValid())
			{
				m_pTexture->Unbind(a_pCommandList);
			}
		}

		//---------------------------------------------------------------------
		void SpriteComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			std::string texPath = m_pTexture->GetName();
			std::string meshPath = m_pMesh->GetName();
			std::string vertexShaderPath = m_pShader->GetVertexPath();
			std::string pixelShaderPath = m_pShader->GetPixelPath();
			a_Document.AddMember(
				JSON_MESH_COMPONENT_TEX_VAR,
				rapidjson::Value(texPath.c_str(), a_Allocator),
				a_Allocator
			);

			a_Document.AddMember(JSON_MESH_COMPONENT_SHADER_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_MESH_COMPONENT_SHADER_VAR].AddMember(
				JSON_MESH_COMPONENT_SHADER_VERTEX_VAR,
				rapidjson::Value(vertexShaderPath.c_str(), a_Allocator),
				a_Allocator
			);

			a_Document[JSON_MESH_COMPONENT_SHADER_VAR].AddMember(
				JSON_MESH_COMPONENT_SHADER_PIXEL_VAR,
				rapidjson::Value(pixelShaderPath.c_str(), a_Allocator),
				a_Allocator
			);

			a_Document.AddMember(
				JSON_MESH_COMPONENT_MESH_VAR,
				rapidjson::Value(meshPath.c_str(), a_Allocator),
				a_Allocator
			);
		}

		//---------------------------------------------------------------------
		void SpriteComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			std::string texPath;
			std::string meshPath;
			std::string shaderPathVertex;
			std::string shaderPathPixel;
			if (a_Document.HasMember(JSON_MESH_COMPONENT_TEX_VAR) && a_Document[JSON_MESH_COMPONENT_TEX_VAR].IsString())
			{
				texPath = a_Document[JSON_MESH_COMPONENT_TEX_VAR].GetString();
			}

			if (a_Document.HasMember(JSON_MESH_COMPONENT_SHADER_VAR) && a_Document[JSON_MESH_COMPONENT_SHADER_VAR].IsString())
			{
				shaderPathVertex = a_Document[JSON_MESH_COMPONENT_SHADER_VAR].GetString();
				shaderPathPixel = a_Document[JSON_MESH_COMPONENT_SHADER_VAR].GetString();
			}

			if (a_Document.HasMember(JSON_MESH_COMPONENT_MESH_VAR) && a_Document[JSON_MESH_COMPONENT_MESH_VAR].IsString())
			{
				meshPath = a_Document[JSON_MESH_COMPONENT_MESH_VAR].GetString();
			}

			std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
			std::shared_ptr<graphics::dx12::CommandList> cCommandList = cCommandQueue->GetCommandList();
			if (!meshPath.empty())
			{
				SetMesh(core::TOOL->GetResourceAtlas().LoadMesh(meshPath, cCommandList));
			}
			if (!texPath.empty())
			{
				SetTexture(core::TOOL->GetResourceAtlas().LoadTexture(texPath, cCommandList));
			}
			if (!shaderPathVertex.empty() && !shaderPathPixel.empty())
			{
				SetShader(core::TOOL->GetResourceAtlas().LoadShader(shaderPathVertex, shaderPathPixel));
			}
			cCommandQueue->ExecuteCommandList(cCommandList);
			cCommandQueue->Flush();
		}
	}
}