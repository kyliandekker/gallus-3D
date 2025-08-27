#include "ResourceAtlas.h"

#include <algorithm>

#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
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
					// Skip resources that are Editor or Missing
					auto category = res->GetResourceCategory();
					if (category == EngineResourceCategory::Missing)
						continue;

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
		std::shared_ptr<graphics::dx12::Shader> ResourceAtlas::LoadShader(const std::string& a_sVertexShader, const std::string& a_sPixelShader)
		{
			std::shared_ptr<graphics::dx12::Shader> shader = GetResource(m_aShaders, a_sVertexShader, fs::path());
			if (!shader->IsValid())
			{
//#ifdef _EDITOR
				fs::path vertexShaderPath = fs::path(m_sResourceFolder + "/shaders/" + a_sVertexShader).lexically_normal();
				fs::path pixelShaderPath = fs::path(m_sResourceFolder + "/shaders/" + a_sPixelShader).lexically_normal();
//
				shader->LoadByPath(vertexShaderPath, pixelShaderPath);
//#else
//				shader->LoadByName(a_sVertexShader, a_sPixelShader);
//#endif // _EDITOR
			}
			return shader;
		}

		//---------------------------------------------------------------------
		bool ResourceAtlas::HasShader(const std::string& a_sName)
		{
			return HasResource(m_aShaders, a_sName, fs::path());
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::Mesh> ResourceAtlas::LoadMesh(const std::string& a_sName, std::shared_ptr<graphics::dx12::CommandList> a_pCommandList)
		{
			std::shared_ptr<graphics::dx12::Mesh> mesh = GetResource(m_aMeshes, a_sName, fs::path());
			if (!mesh->IsValid())
			{
				mesh->LoadByName(a_sName, a_pCommandList);
			}
			return mesh;
		}

		//---------------------------------------------------------------------
		bool ResourceAtlas::HasMesh(const std::string& a_sName)
		{
			return HasResource(m_aMeshes, a_sName, fs::path());
		}

		//---------------------------------------------------------------------
		std::shared_ptr<graphics::dx12::Shader> ResourceAtlas::GetDefaultShader()
		{
			return m_aShaders[MISSING];
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
				if (texture && !texture->IsValid())
				{
					m_aTextures[i] = nullptr;
				}
			}
		}

		//---------------------------------------------------------------------
		const std::vector<std::shared_ptr<graphics::dx12::Texture>>& ResourceAtlas::GetTextures() const
		{
			return m_aTextures;
		}

		//---------------------------------------------------------------------
		const std::vector<std::shared_ptr<graphics::dx12::Shader>>& ResourceAtlas::GetShaders() const
		{
			return m_aShaders;
		}

		//---------------------------------------------------------------------
		const std::vector<std::shared_ptr<graphics::dx12::Mesh>>& ResourceAtlas::GetMeshes() const
		{
			return m_aMeshes;
		}
	}
}