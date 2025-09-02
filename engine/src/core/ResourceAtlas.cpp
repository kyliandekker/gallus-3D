#include "ResourceAtlas.h"

#include <algorithm>

// graphics includes
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12ShaderBind.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/CommandList.h"

namespace gallus
{
	namespace core
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
		std::shared_ptr<graphics::dx12::Texture> ResourceAtlas::LoadTexture(const std::string& a_sName, std::shared_ptr<graphics::dx12::CommandList> a_pCommandList)
		{
			std::shared_ptr<graphics::dx12::Texture> texture = GetResource(m_aTextures, a_sName, fs::path());
			if (!texture->IsValid())
			{
//#ifdef _EDITOR
				fs::path texturePath = fs::path(m_sResourceFolder + "/textures/" + a_sName).lexically_normal();
				texture->LoadByPath(texturePath, a_pCommandList);
//#else
//				texture->LoadByName(a_sName, a_pCommandList);
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
				fs::path pixelShaderPath = fs::path(m_sResourceFolder + "/shaders/" + a_sName).lexically_normal();

				shader->LoadByPath(pixelShaderPath);
//#else
//				shader->LoadByName(a_sName);
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
				fs::path vertexShaderPath = fs::path(m_sResourceFolder + "/shaders/" + a_sName).lexically_normal();

				shader->LoadByPath(vertexShaderPath);
//#else
//				shader->LoadByName(a_sName);
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
		std::shared_ptr<graphics::dx12::DX12ShaderBind> ResourceAtlas::LoadShaderBind(const graphics::dx12::PixelShader* a_PixelShader, const graphics::dx12::VertexShader* a_VertexShader)
		{
			for (std::shared_ptr<graphics::dx12::DX12ShaderBind>& shaderBind : m_aShaderBinds)
			{
				if (shaderBind->HasPixelShader(a_PixelShader) && shaderBind->HasVertexShader(a_VertexShader))
				{
					return shaderBind;
				}
			}

			std::shared_ptr<graphics::dx12::DX12ShaderBind> shaderBind = std::make_shared<graphics::dx12::DX12ShaderBind>(a_PixelShader, a_VertexShader);
			shaderBind->CreatePipelineState();
			m_aShaderBinds.push_back(shaderBind);

			return shaderBind;
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
		std::shared_ptr<graphics::dx12::Texture> ResourceAtlas::GetDefaultTexture()
		{
#ifdef _EDITOR
			return m_aTextures[MISSING + 1];
#else
			return m_aTextures[MISSING];
#endif // _EDITOR
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::Mesh> ResourceAtlas::GetDefaultMesh()
		{
			return m_aMeshes[MISSING];
		}

		//---------------------------------------------------------------------
		void ResourceAtlas::TransitionResources(std::shared_ptr<graphics::dx12::CommandList> a_CommandList)
		{
			for (size_t i = 0; i < m_aTextures.size(); i++)
			{
				std::shared_ptr<graphics::dx12::Texture> texture = m_aTextures[i];
				if (texture && !texture->IsSrvIndexValid())
				{
					texture->CreateSRV(a_CommandList);
				}
			}
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