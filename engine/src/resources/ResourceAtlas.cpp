#include "ResourceAtlas.h"

// external
#include <algorithm>

// core
#include "core/Data.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Material.h"

// animation
#include "animation/Animation.h"

// logger
#include "logger/Logger.h"

// resources
#include "resources/IAssetResource.h"

// gameplay
#include "gameplay/Scene.h"
#include "gameplay/Prefab.h"

namespace gallus::resources
{
	//---------------------------------------------------------------------
	// ResourceAtlas
	//---------------------------------------------------------------------
	template<class T>
	std::shared_ptr<T> ResourceAtlas::GetResource(std::vector<std::shared_ptr<T>>& a_aVector, const std::string& a_sName)
	{
		int32_t index = HasResource(a_aVector, a_sName);

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
			a_aVector[index]->SetName(a_sName);
		}
		else
		{
			a_aVector[index] = std::make_shared<T>();
			a_aVector[index]->SetName(a_sName);
		}

		return a_aVector[index];
	}

	//---------------------------------------------------------------------
	template<class T>
	int32_t ResourceAtlas::HasResource(std::vector<std::shared_ptr<T>>& a_aVector, const std::string& a_sName)
	{
		for (size_t i = 0; i < a_aVector.size(); i++)
		{
			auto& res = a_aVector[i];
			if (res)
			{
				if (res->GetName() == a_sName)
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
		m_aMeshes.reserve(MAX_RESOURCES);
		m_aPrefabs.reserve(MAX_RESOURCES);
		m_aAnimations.reserve(MAX_RESOURCES);
		m_aMaterials.reserve(MAX_RESOURCES);

		LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ECS, "Successfully initialized resource atlas.");
		return System::Initialize();
	}

	//---------------------------------------------------------------------
	bool ResourceAtlas::Destroy()
	{
		LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ECS, "Successfully destroyed resource atlas.");
		return System::Destroy();
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::Texture> ResourceAtlas::LoadTexture(const std::string& a_sName, std::shared_ptr<graphics::dx12::CommandQueue> a_pCommandQueue)
	{
		std::shared_ptr<graphics::dx12::Texture> texture = GetResource(m_aTextures, a_sName);

		if (!texture->IsValid())
		{
			if (!m_pAssetSource)
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load texture \"%s\": Asset Source not set.", a_sName.c_str());
				return texture;
			}

			core::Data data;
			if (!m_pAssetSource->LoadBinary(a_sName, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load texture \"%s\": Could not load binary data.", a_sName.c_str());
				return texture;
			}

			if (!texture->LoadData(data, a_pCommandQueue))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load texture \"%s\": Texture could not load binary data.", a_sName.c_str());
				return texture;
			}
			
			core::Data metaData;
			if (!m_pAssetSource->LoadMetaData(a_sName, true, metaData))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load texture \"%s\": Could not load meta data binary data.", a_sName.c_str());
				return texture;
			}

			if (!texture->LoadMetaData(metaData))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load texture \"%s\": Texture could not load meta data binary data.", a_sName.c_str());
				return texture;
			}
		}
		return texture;
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::Texture> ResourceAtlas::LoadTextureByDescription(const std::string& a_sName, D3D12_RESOURCE_DESC& a_Description)
	{
		std::shared_ptr<graphics::dx12::Texture> texture = GetResource(m_aTextures, a_sName);
		if (!texture->IsValid())
		{
			if (!texture->LoadData(core::Data(), a_Description))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load texture \"%s\".", a_sName.c_str());
			}
		}
		return texture;
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::Texture> ResourceAtlas::LoadTextureEmpty(const std::string& a_sName)
	{
		std::shared_ptr<graphics::dx12::Texture> texture = GetResource(m_aTextures, a_sName);
		return texture;
	}

	//---------------------------------------------------------------------
	std::shared_ptr<graphics::dx12::PixelShader> ResourceAtlas::LoadPixelShader(const std::string& a_sName)
	{
		std::shared_ptr<graphics::dx12::PixelShader> shader = GetResource(m_aPixelShaders, a_sName);
		if (!shader->IsValid())
		{
			if (!m_pAssetSource)
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load pixel shader \"%s\": Asset Source not set.", a_sName.c_str());
				return shader;
			}

			core::Data data;
			if (!m_pAssetSource->LoadBinary(a_sName, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load pixel shader \"%s\".", a_sName.c_str());
				return shader;
			}
			
			if (!shader->LoadData(data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load pixel shader \"%s\".", a_sName.c_str());
				return shader;
			}
		}
		return shader;
	}

	//---------------------------------------------------------------------
	std::shared_ptr<graphics::dx12::VertexShader> ResourceAtlas::LoadVertexShader(const std::string& a_sName)
	{
		std::shared_ptr<graphics::dx12::VertexShader> shader = GetResource(m_aVertexShaders, a_sName);
		if (!shader->IsValid())
		{
			if (!m_pAssetSource)
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load vertex shader \"%s\": Asset Source not set.", a_sName.c_str());
				return shader;
			}

			core::Data data;
			if (!m_pAssetSource->LoadBinary(a_sName, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load vertex shader \"%s\".", a_sName.c_str());
				return shader;
			}
			
			if (!shader->LoadData(data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load vertex shader \"%s\".", a_sName.c_str());
				return shader;
			}
		}
		return shader;
	}

	//---------------------------------------------------------------------
	bool ResourceAtlas::LoadScene(const std::string& a_sName, gameplay::Scene& a_Scene)
	{
		return true;
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::Mesh> ResourceAtlas::LoadMesh(const std::string& a_sName, std::shared_ptr<graphics::dx12::CommandQueue> a_pCommandQueue)
	{
		std::shared_ptr<graphics::dx12::Mesh> mesh = GetResource(m_aMeshes, a_sName);
		if (!mesh->IsValid())
		{
			if (!m_pAssetSource)
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load mesh \"%s\": Asset Source not set.", a_sName.c_str());
				return mesh;
			}

			core::Data data;
			if (!m_pAssetSource->LoadBinary(a_sName, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load mesh \"%s\".", a_sName.c_str());
				return mesh;
			}
			
			if (!mesh->LoadData(data, a_pCommandQueue))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load mesh \"%s\".", a_sName.c_str());
				return mesh;
			}
		}
		return mesh;
	}
	
	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::Mesh> ResourceAtlas::LoadMeshEmpty(const std::string& a_sName)
	{
		std::shared_ptr<graphics::dx12::Mesh> mesh = GetResource(m_aMeshes, a_sName);
		return mesh;
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::Material> ResourceAtlas::LoadMaterial(const std::string& a_sName)
	{
		std::shared_ptr<graphics::dx12::Material> material = GetResource(m_aMaterials, a_sName);
		if (!material->IsValid())
		{
			if (!m_pAssetSource)
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load material \"%s\": Asset Source not set.", a_sName.c_str());
				return material;
			}

			core::Data data;
			if (!m_pAssetSource->LoadBinary(a_sName, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load material \"%s\".", a_sName.c_str());
				return material;
			}
			
			if (!material->LoadData(data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load material \"%s\".", a_sName.c_str());
				return material;
			}
		}
		return material;
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::Material> ResourceAtlas::LoadMaterialEmpty(const std::string& a_sName)
	{
		std::shared_ptr<graphics::dx12::Material> material = GetResource(m_aMaterials, a_sName);
		return material;
	}

	//---------------------------------------------------------------------
	std::weak_ptr<gameplay::Prefab> ResourceAtlas::LoadPrefab(const std::string& a_sName)
	{
		std::shared_ptr<gameplay::Prefab> prefab = GetResource(m_aPrefabs, a_sName);
		if (!prefab->IsValid())
		{
			if (!m_pAssetSource)
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load prefab \"%s\": Asset Source not set.", a_sName.c_str());
				return prefab;
			}

			core::Data data;
			if (!m_pAssetSource->LoadBinary(a_sName, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load prefab \"%s\".", a_sName.c_str());
				return prefab;
			}
			
			if (!prefab->LoadData(data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load prefab \"%s\".", a_sName.c_str());
				return prefab;
			}
		}
		return prefab;
	}

	//---------------------------------------------------------------------
	std::weak_ptr<animation::Animation> ResourceAtlas::LoadAnimation(const std::string& a_sName)
	{
		std::shared_ptr<animation::Animation> animation = GetResource(m_aAnimations, a_sName);
		if (!animation->IsValid())
		{
			if (!m_pAssetSource)
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load animation \"%s\": Asset Source not set.", a_sName.c_str());
				return animation;
			}

			core::Data data;
			if (!m_pAssetSource->LoadBinary(a_sName, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load animation \"%s\".", a_sName.c_str());
				return animation;
			}
			
			if (!animation->LoadData(data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Failed to load animation \"%s\".", a_sName.c_str());
				return animation;
			}
		}
		return animation;
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::PixelShader> ResourceAtlas::GetDefaultPixelShader()
	{
		return m_aPixelShaders[MISSING];
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::VertexShader> ResourceAtlas::GetDefaultVertexShader()
	{
		return m_aVertexShaders[MISSING];
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::Texture> ResourceAtlas::GetDefaultTexture()
	{
		return m_aTextures[MISSING + 1];
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::Mesh> ResourceAtlas::GetDefaultMesh()
	{
		return m_aMeshes[MISSING];
	}

	//---------------------------------------------------------------------
	std::weak_ptr<graphics::dx12::Material> ResourceAtlas::GetDefaultMaterial()
	{
		return m_aMaterials[MISSING];
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
	const std::vector<std::shared_ptr<graphics::dx12::Mesh>>& ResourceAtlas::GetMeshes() const
	{
		return m_aMeshes;
	}

	//---------------------------------------------------------------------
	const std::vector<std::shared_ptr<graphics::dx12::Material>>& ResourceAtlas::GetMaterials() const
	{
		return m_aMaterials;
	}

	//---------------------------------------------------------------------
	const std::vector<std::shared_ptr<animation::Animation>>& ResourceAtlas::GetAnimations() const
	{
		return m_aAnimations;
	}

	//---------------------------------------------------------------------
	std::vector<std::weak_ptr<resources::EngineResource>> ResourceAtlas::GetResourcesOfType(resources::AssetType a_AssetType, resources::EngineResourceCategory a_Category)
	{
		std::vector<std::weak_ptr<resources::EngineResource>> engineResources;
		switch (a_AssetType)
		{
			case resources::AssetType::Animation:
			{
				for (std::shared_ptr<animation::Animation>& resource : m_aAnimations)
				{
					if (a_Category == resources::EngineResourceCategory::Unknown || resource->GetResourceCategory() == a_Category)
					{
						engineResources.push_back(resource);
					}
				}
				break;
			}
			case resources::AssetType::Mesh:
			{
				for (std::shared_ptr<graphics::dx12::Mesh>& resource : m_aMeshes)
				{
					if (a_Category == resources::EngineResourceCategory::Unknown || resource->GetResourceCategory() == a_Category)
					{
						engineResources.push_back(resource);
					}
				}
				break;
			}
			case resources::AssetType::Material:
			{
				for (std::shared_ptr<graphics::dx12::Material>& resource : m_aMaterials)
				{
					if (a_Category == resources::EngineResourceCategory::Unknown || resource->GetResourceCategory() == a_Category)
					{
						engineResources.push_back(resource);
					}
				}
				break;
			}
			case resources::AssetType::PixelShader:
			{
				for (std::shared_ptr<graphics::dx12::PixelShader>& resource : m_aPixelShaders)
				{
					if (a_Category == resources::EngineResourceCategory::Unknown || resource->GetResourceCategory() == a_Category)
					{
						engineResources.push_back(resource);
					}
				}
				break;
			}
			case resources::AssetType::VertexShader:
			{
				for (std::shared_ptr<graphics::dx12::VertexShader>& resource : m_aVertexShaders)
				{
					if (a_Category == resources::EngineResourceCategory::Unknown || resource->GetResourceCategory() == a_Category)
					{
						engineResources.push_back(resource);
					}
				}
				break;
			}
			case resources::AssetType::Texture:
			{
				for (std::shared_ptr<graphics::dx12::Texture>& resource : m_aTextures)
				{
					if (a_Category == resources::EngineResourceCategory::Unknown || resource->GetResourceCategory() == a_Category)
					{
						engineResources.push_back(resource);
					}
				}
				break;
			}
			case resources::AssetType::Prefab:
			{
				for (std::shared_ptr<gameplay::Prefab>& resource : m_aPrefabs)
				{
					if (a_Category == resources::EngineResourceCategory::Unknown || resource->GetResourceCategory() == a_Category)
					{
						engineResources.push_back(resource);
					}
				}
				break;
			}
		}
		return engineResources;
	}

	//---------------------------------------------------------------------
	std::weak_ptr<resources::IAssetSource> ResourceAtlas::GetAssetSource()
	{
		return m_pAssetSource;
	}
}