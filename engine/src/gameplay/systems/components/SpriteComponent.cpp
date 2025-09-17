#include "gameplay/systems/components/SpriteComponent.h"

// core includes
#include "core/Tool.h"

// graphics includes
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/DX12ShaderBind.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12Transform.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

// gameplay includes
#include "gameplay/systems/TransformSystem.h"

#define JSON_MESH_COMPONENT_TEX_VAR "texture"
#define JSON_MESH_COMPONENT_TEX_NAME_VAR "name"
#define JSON_MESH_COMPONENT_TEX_SPRITE_INDEX_VAR "spriteIndex"
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
			m_pShaderBind = core::TOOL->GetResourceAtlas().GetDefaultShaderBind().get();
			m_pTexture = core::TOOL->GetResourceAtlas().GetDefaultTexture().get();
			m_pMesh = core::TOOL->GetResourceAtlas().GetDefaultMesh().get();
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetMesh(graphics::dx12::Mesh* a_pMesh)
		{
			m_pMesh = a_pMesh;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetShader(graphics::dx12::DX12ShaderBind* a_pShaderBind)
		{
			m_pShaderBind = a_pShaderBind;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetTexture(graphics::dx12::Texture* a_pTexture)
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
				m_pTexture->Bind(a_pCommandList, m_iSpriteIndex);
			}

			if (m_pShaderBind && m_pShaderBind->IsValid())
			{
				m_pShaderBind->Bind(a_pCommandList);
			}

			if (m_pMesh && m_pMesh->IsValid())
			{
				graphics::dx12::DX12Transform transform;
				TransformSystem& transformSys = core::TOOL->GetECS().GetSystem<TransformSystem>();
				if (transformSys.HasComponent(a_EntityID))
				{
					transform = transformSys.GetComponent(a_EntityID).Transform();
				}
				DirectX::XMMATRIX mvpMatrix = transform.GetWorldMatrixWithPivot() * viewMatrix * projectionMatrix;
				m_pMesh->Render(a_pCommandList, mvpMatrix);
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

			std::string tex = m_pTexture->GetName();
			std::string mesh = m_pMesh->GetName();
			std::string pixelShader = m_pShaderBind->GetVertexShader()->GetName();
			std::string vertexShader = m_pShaderBind->GetPixelShader()->GetName();
			a_Document.AddMember(JSON_MESH_COMPONENT_TEX_VAR, rapidjson::Value().SetObject(), a_Allocator);
			a_Document[JSON_MESH_COMPONENT_TEX_VAR];
			a_Document[JSON_MESH_COMPONENT_TEX_VAR].AddMember(
				JSON_MESH_COMPONENT_TEX_NAME_VAR,
				rapidjson::Value(tex.c_str(), a_Allocator),
				a_Allocator
			);
			a_Document[JSON_MESH_COMPONENT_TEX_VAR].AddMember(
				JSON_MESH_COMPONENT_TEX_SPRITE_INDEX_VAR,
				m_iSpriteIndex,
				a_Allocator
			);

			a_Document.AddMember(JSON_MESH_COMPONENT_SHADER_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_MESH_COMPONENT_SHADER_VAR].AddMember(
				JSON_MESH_COMPONENT_SHADER_PIXEL_VAR,
				rapidjson::Value(pixelShader.c_str(), a_Allocator),
				a_Allocator
			);

			a_Document[JSON_MESH_COMPONENT_SHADER_VAR].AddMember(
				JSON_MESH_COMPONENT_SHADER_VERTEX_VAR,
				rapidjson::Value(vertexShader.c_str(), a_Allocator),
				a_Allocator
			);

			a_Document.AddMember(
				JSON_MESH_COMPONENT_MESH_VAR,
				rapidjson::Value(mesh.c_str(), a_Allocator),
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

			std::string tex;
			std::string mesh;
			std::string pixelShader;
			std::string vertexShader;
			if (a_Document.HasMember(JSON_MESH_COMPONENT_TEX_VAR) && a_Document[JSON_MESH_COMPONENT_TEX_VAR].IsObject())
			{
				if (a_Document[JSON_MESH_COMPONENT_TEX_VAR].HasMember(JSON_MESH_COMPONENT_TEX_NAME_VAR) && a_Document[JSON_MESH_COMPONENT_TEX_VAR][JSON_MESH_COMPONENT_TEX_NAME_VAR].IsString())
				{
					tex = a_Document[JSON_MESH_COMPONENT_TEX_VAR][JSON_MESH_COMPONENT_TEX_NAME_VAR].GetString();
				}
				if (a_Document[JSON_MESH_COMPONENT_TEX_VAR].HasMember(JSON_MESH_COMPONENT_TEX_SPRITE_INDEX_VAR) && a_Document[JSON_MESH_COMPONENT_TEX_VAR][JSON_MESH_COMPONENT_TEX_SPRITE_INDEX_VAR].IsInt())
				{
					m_iSpriteIndex = a_Document[JSON_MESH_COMPONENT_TEX_VAR][JSON_MESH_COMPONENT_TEX_SPRITE_INDEX_VAR].GetInt();
				}
			}

			if (a_Document.HasMember(JSON_MESH_COMPONENT_SHADER_VAR) && a_Document[JSON_MESH_COMPONENT_SHADER_VAR].IsString())
			{
				pixelShader = a_Document[JSON_MESH_COMPONENT_SHADER_VAR].GetString();
				vertexShader = a_Document[JSON_MESH_COMPONENT_SHADER_VAR].GetString();
			}

			if (a_Document.HasMember(JSON_MESH_COMPONENT_MESH_VAR) && a_Document[JSON_MESH_COMPONENT_MESH_VAR].IsString())
			{
				mesh = a_Document[JSON_MESH_COMPONENT_MESH_VAR].GetString();
			}

			std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
			std::shared_ptr<graphics::dx12::CommandList> cCommandList = cCommandQueue->GetCommandList();
			if (!mesh.empty())
			{
				SetMesh(core::TOOL->GetResourceAtlas().LoadMesh(mesh).get());
			}
			if (!tex.empty())
			{
				SetTexture(core::TOOL->GetResourceAtlas().LoadTexture(tex, cCommandList).get());
			}
			if (!vertexShader.empty() && !pixelShader.empty())
			{
				SetShader(core::TOOL->GetResourceAtlas().LoadShaderBind(
					core::TOOL->GetResourceAtlas().LoadPixelShader(pixelShader).get(),
					core::TOOL->GetResourceAtlas().LoadVertexShader(vertexShader).get()
				).get());
			}
			cCommandQueue->ExecuteCommandList(cCommandList);
			cCommandQueue->Flush();
		}
	}
}