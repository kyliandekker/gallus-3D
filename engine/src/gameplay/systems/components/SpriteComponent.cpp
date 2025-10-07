#include "gameplay/systems/components/SpriteComponent.h"

#include <rapidjson/utils.h>

// core includes
#include "core/Engine.h"

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

#define JSON_SPRITE_COMPONENT_TEX_VAR "texture"
#define JSON_SPRITE_COMPONENT_TEX_NAME_VAR "name"
#define JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR "spriteIndex"
#define JSON_SPRITE_COMPONENT_MESH_VAR "mesh"
#define JSON_SPRITE_COMPONENT_SHADER_VAR "shader"
#define JSON_SPRITE_COMPONENT_SHADER_PIXEL_VAR "pixel"
#define JSON_SPRITE_COMPONENT_SHADER_VERTEX_VAR "vertex"
#define JSON_SPRITE_COMPONENT_MATERIAL_VAR "material"
#define JSON_SPRITE_COMPONENT_COLOR_VAR "color"
#define JSON_SPRITE_COMPONENT_COLOR_R_VAR "r"
#define JSON_SPRITE_COMPONENT_COLOR_G_VAR "g"
#define JSON_SPRITE_COMPONENT_COLOR_B_VAR "b"
#define JSON_SPRITE_COMPONENT_COLOR_A_VAR "a"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// SpriteComponent
		//---------------------------------------------------------------------
		void SpriteComponent::Init(const gameplay::EntityID& a_EntityID)
		{
			Component::Init(a_EntityID);
			m_pShaderBind = core::ENGINE->GetResourceAtlas().GetDefaultShaderBind().get();
			m_pTexture = core::ENGINE->GetResourceAtlas().GetDefaultTexture().get();
			m_pMesh = core::ENGINE->GetResourceAtlas().GetDefaultMesh().get();
			m_vColor = { 1, 1, 1, 1 };
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
		bool CheckVisibility(const gallus::graphics::dx12::DX12Transform& a_Transform, const graphics::dx12::Camera& a_Camera)
		{
			const DirectX::XMFLOAT2& pos = a_Transform.GetPosition();
			const DirectX::XMFLOAT2& scale = a_Transform.GetScale();
			const DirectX::XMFLOAT2& pivot = a_Transform.GetPivot();
			float rotation = a_Transform.GetRotation();

			DirectX::XMFLOAT2 corners[4] = {
				{ (-0.5f) * scale.x, (-0.5f) * scale.y }, // top-left
				{ (0.5f) * scale.x, (-0.5f) * scale.y }, // top-right
				{ (0.5f) * scale.x, (0.5f) * scale.y }, // bottom-right
				{ (-0.5f) * scale.x, (0.5f) * scale.y }  // bottom-left
			};

			// Rotate corners around pivot and translate to world position
			float cosR = cos(rotation);
			float sinR = sin(rotation);

			for (int i = 0; i < 4; ++i)
			{
				// relative to pivot
				float x = corners[i].x - pivot.x * scale.x;
				float y = corners[i].y - pivot.y * scale.y;

				// rotate
				float rx = x * cosR - y * sinR;
				float ry = x * sinR + y * cosR;

				// translate to world
				corners[i].x = rx + pos.x;
				corners[i].y = ry + pos.y;
			}

			// Compute AABB of rotated rectangle
			float minX = corners[0].x, maxX = corners[0].x;
			float minY = corners[0].y, maxY = corners[0].y;

			for (int i = 1; i < 4; ++i)
			{
				if (corners[i].x < minX)
				{
					minX = corners[i].x;
				}
				if (corners[i].x > maxX)
				{
					maxX = corners[i].x;
				}
				if (corners[i].y < minY)
				{
					minY = corners[i].y;
				}
				if (corners[i].y > maxY)
				{
					maxY = corners[i].y;
				}
			}

			// Check visibility
			if (maxX < 0.0f || minX > a_Camera.GetSize().x ||
				maxY < 0.0f || minY > a_Camera.GetSize().y)
			{
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera)
		{
			const DirectX::XMMATRIX viewMatrix = a_Camera.GetViewMatrix();
			const DirectX::XMMATRIX& projectionMatrix = a_Camera.GetProjectionMatrix();

			graphics::dx12::DX12Transform transform;
			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
			if (transformSys.HasComponent(a_EntityID))
			{
				transform = transformSys.GetComponent(a_EntityID).Transform();
			}
			DirectX::XMMATRIX mvpMatrix = transform.GetWorldMatrixWithPivot() * viewMatrix * projectionMatrix;

			if (!CheckVisibility(transform, a_Camera))
			{
				return;
			}

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

			{
				a_Document.AddMember(JSON_SPRITE_COMPONENT_TEX_VAR, rapidjson::Value().SetObject(), a_Allocator);
				std::string tex = m_pTexture->GetName();
				a_Document[JSON_SPRITE_COMPONENT_TEX_VAR].AddMember(
					JSON_SPRITE_COMPONENT_TEX_NAME_VAR,
					rapidjson::Value(tex.c_str(), a_Allocator),
					a_Allocator
				);
				a_Document[JSON_SPRITE_COMPONENT_TEX_VAR].AddMember(
					JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR,
					m_iSpriteIndex,
					a_Allocator
				);
			}

			a_Document.AddMember(JSON_SPRITE_COMPONENT_SHADER_VAR, rapidjson::Value().SetObject(), a_Allocator);

			std::string pixelShader = m_pShaderBind->GetPixelShader()->GetName();
			a_Document[JSON_SPRITE_COMPONENT_SHADER_VAR].AddMember(
				JSON_SPRITE_COMPONENT_SHADER_PIXEL_VAR,
				rapidjson::Value(pixelShader.c_str(), a_Allocator),
				a_Allocator
			);

			std::string vertexShader = m_pShaderBind->GetVertexShader()->GetName();
			a_Document[JSON_SPRITE_COMPONENT_SHADER_VAR].AddMember(
				JSON_SPRITE_COMPONENT_SHADER_VERTEX_VAR,
				rapidjson::Value(vertexShader.c_str(), a_Allocator),
				a_Allocator
			);

			std::string mesh = m_pMesh->GetName();
			a_Document.AddMember(
				JSON_SPRITE_COMPONENT_MESH_VAR,
				rapidjson::Value(mesh.c_str(), a_Allocator),
				a_Allocator
			);

			{
				rapidjson::Document colorDoc;
				colorDoc.SetObject();

				colorDoc.AddMember(JSON_SPRITE_COMPONENT_COLOR_R_VAR, m_vColor.x, a_Allocator);
				colorDoc.AddMember(JSON_SPRITE_COMPONENT_COLOR_G_VAR, m_vColor.y, a_Allocator);
				colorDoc.AddMember(JSON_SPRITE_COMPONENT_COLOR_B_VAR, m_vColor.z, a_Allocator);
				colorDoc.AddMember(JSON_SPRITE_COMPONENT_COLOR_A_VAR, m_vColor.w, a_Allocator);

				a_Document.AddMember(
					JSON_SPRITE_COMPONENT_COLOR_VAR,
					colorDoc,
					a_Allocator
				);
			}
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
			if (a_Document.HasMember(JSON_SPRITE_COMPONENT_TEX_VAR) && a_Document[JSON_SPRITE_COMPONENT_TEX_VAR].IsObject())
			{
				if (a_Document[JSON_SPRITE_COMPONENT_TEX_VAR].HasMember(JSON_SPRITE_COMPONENT_TEX_NAME_VAR) && a_Document[JSON_SPRITE_COMPONENT_TEX_VAR][JSON_SPRITE_COMPONENT_TEX_NAME_VAR].IsString())
				{
					rapidjson::GetString(a_Document[JSON_SPRITE_COMPONENT_TEX_VAR], JSON_SPRITE_COMPONENT_TEX_NAME_VAR, tex);
				}
				if (a_Document[JSON_SPRITE_COMPONENT_TEX_VAR].HasMember(JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR) && a_Document[JSON_SPRITE_COMPONENT_TEX_VAR][JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR].IsInt())
				{
					int spriteIndex = 0;
					rapidjson::GetInt(a_Document[JSON_SPRITE_COMPONENT_TEX_VAR], JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR, spriteIndex);
					m_iSpriteIndex = spriteIndex;
				}
			}

			if (a_Document.HasMember(JSON_SPRITE_COMPONENT_SHADER_VAR) && a_Document[JSON_SPRITE_COMPONENT_SHADER_VAR].IsObject())
			{
				rapidjson::GetString(a_Document[JSON_SPRITE_COMPONENT_SHADER_VAR], JSON_SPRITE_COMPONENT_SHADER_PIXEL_VAR, pixelShader);
				rapidjson::GetString(a_Document[JSON_SPRITE_COMPONENT_SHADER_VAR], JSON_SPRITE_COMPONENT_SHADER_VERTEX_VAR, vertexShader);
			}

			if (a_Document.HasMember(JSON_SPRITE_COMPONENT_MESH_VAR) && a_Document[JSON_SPRITE_COMPONENT_MESH_VAR].IsString())
			{
				rapidjson::GetString(a_Document, JSON_SPRITE_COMPONENT_MESH_VAR, mesh);
			}

			if (a_Document.HasMember(JSON_SPRITE_COMPONENT_COLOR_VAR) && a_Document[JSON_SPRITE_COMPONENT_COLOR_VAR].IsObject())
			{
				rapidjson::GetFloat(a_Document[JSON_SPRITE_COMPONENT_COLOR_VAR], JSON_SPRITE_COMPONENT_COLOR_R_VAR, m_vColor.x);
				rapidjson::GetFloat(a_Document[JSON_SPRITE_COMPONENT_COLOR_VAR], JSON_SPRITE_COMPONENT_COLOR_G_VAR, m_vColor.y);
				rapidjson::GetFloat(a_Document[JSON_SPRITE_COMPONENT_COLOR_VAR], JSON_SPRITE_COMPONENT_COLOR_B_VAR, m_vColor.z);
				rapidjson::GetFloat(a_Document[JSON_SPRITE_COMPONENT_COLOR_VAR], JSON_SPRITE_COMPONENT_COLOR_A_VAR, m_vColor.w);
			}

			std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = core::ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
			std::shared_ptr<graphics::dx12::CommandList> cCommandList = cCommandQueue->GetCommandList();
			if (!mesh.empty())
			{
				SetMesh(core::ENGINE->GetResourceAtlas().LoadMesh(mesh).get());
			}
			if (!tex.empty())
			{
				SetTexture(core::ENGINE->GetResourceAtlas().LoadTexture(tex, cCommandList).get());
			}
			if (!vertexShader.empty() && !pixelShader.empty())
			{
				SetShader(core::ENGINE->GetResourceAtlas().LoadShaderBind(
					core::ENGINE->GetResourceAtlas().LoadPixelShader(pixelShader).get(),
					core::ENGINE->GetResourceAtlas().LoadVertexShader(vertexShader).get()
				).get());
			}
			cCommandQueue->ExecuteCommandList(cCommandList);
			cCommandQueue->Flush();
		}
	}
}