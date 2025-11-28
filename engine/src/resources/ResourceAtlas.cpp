// header
#include "ResourceAtlas.h"

// external
#include <algorithm>

// core
#include "core/Data.h"
#include "core/ArgProcessor.h"

// graphics includes
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12ShaderBind.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/Scene.h"
#include "gameplay/Prefab.h"

namespace gallus
{
	namespace resources
	{
		//---------------------------------------------------------------------
		// ResourceAtlas
		//---------------------------------------------------------------------
		template<class T>
		std::shared_ptr<T> ResourceAtlas::GetResource(std::vector<std::shared_ptr<T>>& a_aVector, const std::string& a_sName, const fs::path& a_Path)
		{
			int32_t index = HasResource(a_aVector, a_sName, a_Path);

			// Resource exists → Return it immediately.
			if (index != -1)
			{
				return a_aVector[index];
			}

			// Look for an empty slot.
			for (size_t i = 0; i < a_aVector.size(); i++)
			{
				if (!a_aVector[i] || !a_aVector[i]->IsValid())
				{
					index = static_cast<int32_t>(i);
					break;
				}
			}

			// If no empty slots were found, push a new one.
			if (index == -1)
			{
				// Check if adding exceeds MAX_RESOURCES
				if (a_aVector.size() >= MAX_RESOURCES)
				{
					return a_aVector[MISSING]; // Return missing resource
				}

				index = static_cast<int32_t>(a_aVector.size());
				a_aVector.push_back(std::make_shared<T>());
			}
			else
			{
				a_aVector[index] = std::make_shared<T>();
			}

			return a_aVector[index];
		}

		//---------------------------------------------------------------------
		template<class T>
		int32_t ResourceAtlas::HasResource(std::vector<std::shared_ptr<T>>& a_aVector, const std::string& a_sName, const fs::path& a_Path)
		{
			for (size_t i = 0; i < a_aVector.size(); i++)
			{
				auto& res = a_aVector[i];
				if (res)
				{
					if (res->IsUnique())
					{
						continue;
					}

					if (res->GetName() == a_sName || (!a_Path.empty() && a_Path == res->GetPath()))
					{
						return static_cast<int32_t>(i);
					}
				}
			}
			return -1;
		}

		//---------------------------------------------------------------------
		bool ResourceAtlas::Initialize()
		{
			m_aTextures.reserve(MAX_RESOURCES);
			m_aPixelShaders.reserve(MAX_RESOURCES);
			m_aVertexShaders.reserve(MAX_RESOURCES);
			m_aShaderBinds.reserve(MAX_RESOURCES);
			m_aMeshes.reserve(MAX_RESOURCES);

			std::string resourceFolder = core::ARGS.GetArgument<std::string>(ASSET_PATH_ARG);
			if (resourceFolder.empty())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Assets folder has not been set. Please add the argument: \"assetPath=\"your/path/to/assets/folder\"\".");
			}

			m_ResourceFolder.SetPath(resourceFolder);
			m_ResourceFolder.Scan();

			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ECS, "Resource atlas initialized.");
			return System::Initialize();
		}

		//---------------------------------------------------------------------
		bool ResourceAtlas::Destroy()
		{
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ECS, "Resource atlas destroyed.");
			return System::Destroy();
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::Texture> ResourceAtlas::LoadTexture(const std::string& a_sName, std::shared_ptr<graphics::dx12::CommandQueue> a_pCommandQueue)
		{
			std::shared_ptr<graphics::dx12::Texture> texture = GetResource(m_aTextures, a_sName, fs::path());
			if (!texture->IsValid())
			{
//#ifdef _EDITOR
				resources::FileResource* fileResource = nullptr;
				if (!GetResource(a_sName, fileResource))
				{
					LOG(LogSeverity::LOGSEVERITY_ERROR, "Could not find resource: \"%s\".", a_sName.c_str());
					return nullptr;
				}

				fs::path texturePath = fileResource->GetPath().lexically_normal();
				texture->LoadByPath(texturePath, a_pCommandQueue);
//#else
// 
//#endif // _EDITOR
			}
			return texture;
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::Texture> ResourceAtlas::LoadTextureByDescription(const std::string& a_sName, D3D12_RESOURCE_DESC& a_Description)
		{
			std::shared_ptr<graphics::dx12::Texture> texture = GetResource(m_aTextures, a_sName, fs::path());
			if (!texture->IsValid())
			{
				texture->LoadByName(a_sName, a_Description);
			}
			return texture;
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::Texture> ResourceAtlas::LoadTextureEmpty(const std::string& a_sName)
		{
			std::shared_ptr<graphics::dx12::Texture> texture = GetResource(m_aTextures, a_sName, fs::path());
			return texture;
		}

		//---------------------------------------------------------------------
		bool ResourceAtlas::HasTexture(const std::string& a_sName)
		{
			return HasResource(m_aTextures, a_sName, fs::path());
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::PixelShader> ResourceAtlas::LoadPixelShader(const std::string& a_sName)
		{
			std::shared_ptr<graphics::dx12::PixelShader> shader = GetResource(m_aPixelShaders, a_sName, fs::path());
			if (!shader->IsValid())
			{
//#ifdef _EDITOR
				resources::FileResource* fileResource = nullptr;
				if (!GetResource(a_sName, fileResource))
				{
					LOG(LogSeverity::LOGSEVERITY_ERROR, "Could not find resource: \"%s\".", a_sName.c_str());
					return nullptr;
				}

				fs::path pixelShaderPath = fileResource->GetPath().lexically_normal();
				shader->LoadByPath(pixelShaderPath);
//#else
// 
//#endif // _EDITOR
			}
			return shader;
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::VertexShader> ResourceAtlas::LoadVertexShader(const std::string& a_sName)
		{
			std::shared_ptr<graphics::dx12::VertexShader> shader = GetResource(m_aVertexShaders, a_sName, fs::path());
			if (!shader->IsValid())
			{
//#ifdef _EDITOR
				resources::FileResource* fileResource = nullptr;
				if (!GetResource(a_sName, fileResource))
				{
					LOG(LogSeverity::LOGSEVERITY_ERROR, "Could not find resource: \"%s\".", a_sName.c_str());
					return nullptr;
				}

				fs::path vertexShaderPath = fileResource->GetPath().lexically_normal();
				shader->LoadByPath(vertexShaderPath);
//#else
// 
//#endif // _EDITOR
			}
			return shader;
		}

		//---------------------------------------------------------------------
		bool ResourceAtlas::HasPixelShader(const std::string& a_sName)
		{
			return HasResource(m_aPixelShaders, a_sName, fs::path());
		}

		//---------------------------------------------------------------------
		bool ResourceAtlas::HasVertexShader(const std::string& a_sName)
		{
			return HasResource(m_aVertexShaders, a_sName, fs::path());
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::DX12ShaderBind> ResourceAtlas::LoadShaderBind(const std::string& a_sName, const graphics::dx12::PixelShader* a_PixelShader, const graphics::dx12::VertexShader* a_VertexShader)
		{
			for (std::shared_ptr<graphics::dx12::DX12ShaderBind>& shaderBind : m_aShaderBinds)
			{
				if (shaderBind->HasPixelShader(a_PixelShader) && shaderBind->HasVertexShader(a_VertexShader))
				{
					return shaderBind;
				}
			}

			std::shared_ptr<graphics::dx12::DX12ShaderBind> shaderBind = std::make_shared<graphics::dx12::DX12ShaderBind>();
			shaderBind->LoadByName(a_sName, a_PixelShader, a_VertexShader);
			shaderBind->CreatePipelineState();
			m_aShaderBinds.push_back(shaderBind);

			return shaderBind;
		}

		//---------------------------------------------------------------------
		bool ResourceAtlas::LoadScene(const std::string& a_sName, gameplay::Scene& a_Scene)
		{
//#ifdef _EDITOR
			resources::FileResource* fileResource = nullptr;
			if (!GetResource(a_sName, fileResource))
			{
				LOG(LogSeverity::LOGSEVERITY_ERROR, "Could not find resource: \"%s\".", a_sName.c_str());
				return false;
			}

			fs::path scenePath = fileResource->GetPath();
			a_Scene.LoadByPath(scenePath);

			return true;
//#else
			return false;
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::Mesh> ResourceAtlas::LoadMesh(const std::string& a_sName)
		{
			std::shared_ptr<graphics::dx12::Mesh> mesh = GetResource(m_aMeshes, a_sName, fs::path());
			if (!mesh->IsValid())
			{
				mesh->LoadByName(a_sName);
			}
			return mesh;
		}

		//---------------------------------------------------------------------
		std::shared_ptr<gameplay::Prefab> ResourceAtlas::LoadPrefab(const std::string& a_sName)
		{
			resources::FileResource* fileResource = nullptr;
			if (!GetResource(a_sName, fileResource))
			{
				LOG(LogSeverity::LOGSEVERITY_ERROR, "Could not find resource: \"%s\".", a_sName.c_str());
				return nullptr;
			}

			std::shared_ptr<gameplay::Prefab> prefab = GetResource(m_aPrefabs, a_sName, fileResource->GetPath());
			if (!prefab->IsValid())
			{
				fs::path prefabPath = fileResource->GetPath();
				prefab->LoadByPath(prefabPath);
			}
			return prefab;
		}

		//---------------------------------------------------------------------
		bool ResourceAtlas::HasMesh(const std::string& a_sName)
		{
			return HasResource(m_aMeshes, a_sName, fs::path());
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::PixelShader> ResourceAtlas::GetDefaultPixelShader()
		{
			return m_aPixelShaders[MISSING];
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::VertexShader> ResourceAtlas::GetDefaultVertexShader()
		{
			return m_aVertexShaders[MISSING];
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::DX12ShaderBind> ResourceAtlas::GetDefaultShaderBind()
		{
			return m_aShaderBinds[MISSING];
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::DX12ShaderBind> ResourceAtlas::GetRenderTexShaderBind()
		{
			return m_aShaderBinds[RENDER_TEX];
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::Texture> ResourceAtlas::GetDefaultTexture()
		{
			return m_aTextures[MISSING + 1];
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::Mesh> ResourceAtlas::GetDefaultMesh()
		{
			return m_aMeshes[MISSING];
		}

		//---------------------------------------------------------------------
		const std::vector<std::shared_ptr<graphics::dx12::Texture>>& ResourceAtlas::GetTextures() const
		{
			return m_aTextures;
		}

		//---------------------------------------------------------------------
		const std::vector<std::shared_ptr<graphics::dx12::PixelShader>>& ResourceAtlas::GetPixelShaders() const
		{
			return m_aPixelShaders;
		}

		//---------------------------------------------------------------------
		const std::vector<std::shared_ptr<graphics::dx12::VertexShader>>& ResourceAtlas::GetVertexShaders() const
		{
			return m_aVertexShaders;
		}

		//---------------------------------------------------------------------
		const std::vector<std::shared_ptr<graphics::dx12::DX12ShaderBind>>& ResourceAtlas::GetShaderBinds() const
		{
			return m_aShaderBinds;
		}

		//---------------------------------------------------------------------
		const std::vector<std::shared_ptr<graphics::dx12::Mesh>>& ResourceAtlas::GetMeshes() const
		{
			return m_aMeshes;
		}
	}
}