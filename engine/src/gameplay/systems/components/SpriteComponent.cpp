#include "SpriteComponent.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/DX12ShaderBind.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12Transform.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/systems/TransformSystem.h"

#include "logger/Logger.h"

#define JSON_SPRITE_COMPONENT_TEX_VAR "texture"
#define JSON_SPRITE_COMPONENT_TEX_NAME_VAR "name"
#define JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR "spriteIndex"
#define JSON_SPRITE_COMPONENT_MESH_VAR "mesh"
#define JSON_SPRITE_COMPONENT_SHADER_VAR "shader"
#define JSON_SPRITE_COMPONENT_SHADER_PIXEL_VAR "pixel"
#define JSON_SPRITE_COMPONENT_SHADER_VERTEX_VAR "vertex"
#define JSON_SPRITE_COMPONENT_MATERIAL_VAR "material"
#define JSON_SPRITE_COMPONENT_COLOR_VAR "color"

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

			graphics::dx12::DX12Transform transform;
			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
			if (transformSys.HasComponent(a_EntityID))
			{
				transform = transformSys.GetComponent(a_EntityID).Transform();
			}

			if (transform.GetCameraType() == graphics::dx12::CameraType_Screen)
			{
				if (core::ENGINE->GetDX12().GetCameraIsolationMode() != graphics::dx12::CameraIsolationMode::CameraIsolationMode_2D && core::ENGINE->GetDX12().GetCameraIsolationMode() != graphics::dx12::CameraIsolationMode::CameraIsolationMode_2D3D)
				{
					return;
				}
			}
			else if (transform.GetCameraType() == graphics::dx12::CameraType_World)
			{
				if (core::ENGINE->GetDX12().GetCameraIsolationMode() != graphics::dx12::CameraIsolationMode::CameraIsolationMode_3D && core::ENGINE->GetDX12().GetCameraIsolationMode() != graphics::dx12::CameraIsolationMode::CameraIsolationMode_2D3D)
				{
					return;
				}
			}

			const DirectX::XMMATRIX viewMatrix = a_Camera.GetViewMatrix(transform.GetCameraType());
			const DirectX::XMMATRIX& projectionMatrix = a_Camera.GetProjectionMatrix(transform.GetCameraType());

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
#ifdef _EDITOR
		void SpriteComponent::Serialize(resources::SrcData& a_SrcData) const
		{
			{
				resources::SrcData textureSrc;
				textureSrc.SetObject();
				if (auto tex = m_pTexture.lock())
				{
					std::string texName = tex->GetName();
					textureSrc.SetString(JSON_SPRITE_COMPONENT_TEX_NAME_VAR, texName);
				}
				textureSrc.SetInt(JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR, m_iSpriteIndex);

				a_SrcData.SetSrcObject(JSON_SPRITE_COMPONENT_TEX_VAR, textureSrc);
			}

			{
				resources::SrcData shaderSrc;
				shaderSrc.SetObject();
				if (auto shaderBind = m_pShaderBind.lock())
				{
					if (auto pixelShader = shaderBind->GetPixelShader().lock())
					{
						std::string pixelShaderName = pixelShader->GetName();
						shaderSrc.SetString(JSON_SPRITE_COMPONENT_SHADER_PIXEL_VAR, pixelShaderName);
					}

					if (auto vertexShader = shaderBind->GetVertexShader().lock())
					{
						std::string vertexShaderName = vertexShader->GetName();
						shaderSrc.SetString(JSON_SPRITE_COMPONENT_SHADER_VERTEX_VAR, vertexShaderName);
					}
				}
				a_SrcData.SetSrcObject(JSON_SPRITE_COMPONENT_SHADER_VAR, shaderSrc);
			}

			if (auto mesh = m_pMesh.lock())
			{
				std::string meshName = mesh->GetName();
				a_SrcData.SetString(JSON_SPRITE_COMPONENT_MESH_VAR, meshName);
			}

			a_SrcData.SetColor(JSON_SPRITE_COMPONENT_COLOR_VAR, m_vColor);
		}
#endif

		//---------------------------------------------------------------------
		void SpriteComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			resources::SrcData texComp;
			if (!a_SrcData.GetSrcObject(JSON_SPRITE_COMPONENT_TEX_VAR, texComp))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Sprite component did not have key %s present in its meta data.", JSON_SPRITE_COMPONENT_TEX_VAR);
			}

			std::string tex = "";
			if (!texComp.GetString(JSON_SPRITE_COMPONENT_TEX_NAME_VAR, tex))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Sprite component did not have key %s present in its meta data.", JSON_SPRITE_COMPONENT_TEX_NAME_VAR);
			}

			int textureIndex = 0;
			if (!texComp.GetInt(JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR, textureIndex))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Sprite component did not have key %s present in its meta data.", JSON_SPRITE_COMPONENT_TEX_SPRITE_INDEX_VAR);
			}
			SetSpriteIndex(textureIndex);

			resources::SrcData shaderVar;
			if (!a_SrcData.GetSrcObject(JSON_SPRITE_COMPONENT_SHADER_VAR, shaderVar))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Sprite component did not have key %s present in its meta data.", JSON_SPRITE_COMPONENT_SHADER_VAR);
			}

			std::string pixelShader = "";
			if (!shaderVar.GetString(JSON_SPRITE_COMPONENT_SHADER_PIXEL_VAR, pixelShader))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Sprite component did not have key %s present in its meta data.", JSON_SPRITE_COMPONENT_SHADER_PIXEL_VAR);
			}

			std::string vertexShader = "";
			if (!shaderVar.GetString(JSON_SPRITE_COMPONENT_SHADER_VERTEX_VAR, vertexShader))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Sprite component did not have key %s present in its meta data.", JSON_SPRITE_COMPONENT_SHADER_VERTEX_VAR);
			}

			std::string mesh = "";
			if (!a_SrcData.GetString(JSON_SPRITE_COMPONENT_MESH_VAR, mesh))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Sprite component did not have key %s present in its meta data.", JSON_SPRITE_COMPONENT_MESH_VAR);
			}

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