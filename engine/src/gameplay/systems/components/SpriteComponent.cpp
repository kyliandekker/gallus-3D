#include "SpriteComponent.h"

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
#define JSON_SPRITE_COMPONENT_CAMERA_TYPE_VAR "cameraType"

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

			m_pShaderBind = core::ENGINE->GetResourceAtlas().GetDefaultShaderBind();
			m_pTexture = core::ENGINE->GetResourceAtlas().GetDefaultTexture();
			m_pMesh = core::ENGINE->GetResourceAtlas().GetDefaultMesh();
			m_vColor = { 1, 1, 1, 1 };
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh)
		{
			m_pMesh = a_pMesh;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetShader(std::weak_ptr<graphics::dx12::DX12ShaderBind> a_pShaderBind)
		{
			m_pShaderBind = a_pShaderBind;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetTexture(std::weak_ptr<graphics::dx12::Texture> a_pTexture)
		{
			m_pTexture = a_pTexture;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera)
		{
			if (!core::ENGINE->GetECS().GetEntity(m_EntityID)->IsActive())
			{
				return;
			}

			const DirectX::XMMATRIX viewMatrix = a_Camera.GetViewMatrix(m_CameraType);
			const DirectX::XMMATRIX& projectionMatrix = a_Camera.GetProjectionMatrix(m_CameraType);

			graphics::dx12::DX12Transform2D transform;
			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
			if (transformSys.HasComponent(a_EntityID))
			{
				transform = transformSys.GetComponent(a_EntityID).Transform();
			}
			DirectX::XMMATRIX mvpMatrix = transform.GetWorldMatrixWithPivot() * viewMatrix * projectionMatrix;

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
					tex->Bind(a_pCommandList, m_iSpriteIndex);
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
		graphics::dx12::CameraType SpriteComponent::GetCameraType() const
		{
			return m_CameraType;
		}

		//---------------------------------------------------------------------
		void SpriteComponent::SetCameraMode(graphics::dx12::CameraType a_CameraType)
		{
			m_CameraType = a_CameraType;
		}

		//---------------------------------------------------------------------
#ifdef _EDITOR
		void SpriteComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			{
				if (auto tex = m_pTexture.lock())
				{
					a_Document.AddMember(JSON_SPRITE_COMPONENT_TEX_VAR, rapidjson::Value().SetObject(), a_Allocator);
					std::string texName = tex->GetName();
					a_Document[JSON_SPRITE_COMPONENT_TEX_VAR].AddMember(
						JSON_SPRITE_COMPONENT_TEX_NAME_VAR,
						rapidjson::Value(texName.c_str(), a_Allocator),
						a_Allocator
					);
				}
				a_Document[JSON_SPRITE_COMPONENT_TEX_VAR].AddMember(
					JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR,
					m_iSpriteIndex,
					a_Allocator
				);
			}

			a_Document.AddMember(JSON_SPRITE_COMPONENT_SHADER_VAR, rapidjson::Value().SetObject(), a_Allocator);

			if (auto shaderBind = m_pShaderBind.lock())
			{
				if (auto pixelShader = shaderBind->GetPixelShader().lock())
				{
					std::string pixelShaderName = pixelShader->GetName();
					a_Document[JSON_SPRITE_COMPONENT_SHADER_VAR].AddMember(
						JSON_SPRITE_COMPONENT_SHADER_PIXEL_VAR,
						rapidjson::Value(pixelShaderName.c_str(), a_Allocator),
						a_Allocator
					);
				}
				
				if (auto vertexShader = shaderBind->GetVertexShader().lock())
				{
					std::string vertexShaderName = vertexShader->GetName();
					a_Document[JSON_SPRITE_COMPONENT_SHADER_VAR].AddMember(
						JSON_SPRITE_COMPONENT_SHADER_VERTEX_VAR,
						rapidjson::Value(vertexShaderName.c_str(), a_Allocator),
						a_Allocator
					);
				}
			}

			if (auto mesh = m_pMesh.lock())
			{
				std::string meshName = mesh->GetName();
				a_Document.AddMember(
					JSON_SPRITE_COMPONENT_MESH_VAR,
					rapidjson::Value(meshName.c_str(), a_Allocator),
					a_Allocator
				);
			}

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

			a_Document.AddMember(
				JSON_SPRITE_COMPONENT_CAMERA_TYPE_VAR,
				(int) m_CameraType,
				a_Allocator
			);
		}
#endif

		//---------------------------------------------------------------------
		void SpriteComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			std::string tex = a_SrcData.GetSrc(JSON_SPRITE_COMPONENT_TEX_VAR).GetString(JSON_SPRITE_COMPONENT_TEX_NAME_VAR);
			SetSpriteIndex(a_SrcData.GetSrc(JSON_SPRITE_COMPONENT_TEX_VAR).GetInt(JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR));
			std::string pixelShader = a_SrcData.GetSrc(JSON_SPRITE_COMPONENT_SHADER_VAR).GetString(JSON_SPRITE_COMPONENT_SHADER_PIXEL_VAR);
			std::string vertexShader = a_SrcData.GetSrc(JSON_SPRITE_COMPONENT_SHADER_VAR).GetString(JSON_SPRITE_COMPONENT_SHADER_VERTEX_VAR);
			std::string mesh = a_SrcData.GetString(JSON_SPRITE_COMPONENT_MESH_VAR);

			m_CameraType = a_SrcData.GetEnum<graphics::dx12::CameraType>(JSON_SPRITE_COMPONENT_CAMERA_TYPE_VAR);

			std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = core::ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
			if (!mesh.empty())
			{
				SetMesh(core::ENGINE->GetResourceAtlas().LoadMesh(mesh, cCommandQueue));
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
		/// <param name="a_iSpriteIndex">The index the sprite should have.</param>
		void SpriteComponent::SetSpriteIndex(int8_t a_iSpriteIndex)
		{
			size_t numSpriteRects = 0;
			if (auto tex = m_pTexture.lock())
			{
				numSpriteRects = tex->GetSpriteRectsSize() - 1;
			}
			if (a_iSpriteIndex < 0)
			{
				a_iSpriteIndex = 0;
			}
			else if (a_iSpriteIndex > numSpriteRects)
			{
				a_iSpriteIndex = numSpriteRects;
			}
			m_iSpriteIndex = a_iSpriteIndex;
		}
	}
}