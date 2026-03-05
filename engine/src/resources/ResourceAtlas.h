#pragma once

#include "core/System.h"

// external
#include <vector>
#include <memory>

#include "graphics/dx12/DX12PCH.h"

// resources
#include "resources/IAssetResource.h"

struct D3D12_RESOURCE_DESC;

namespace gallus::graphics::dx12
{
	class Texture;
	class PixelShader;
	class VertexShader;
	class Mesh;
	class Material;

	class CommandQueue;
	class CommandList;
}
namespace gallus::animation
{
	class Animation;
}
namespace gallus::gameplay
{
	class Scene;
	class Prefab;
}
namespace gallus::resources
{
	class EngineResource;
	enum class EngineResourceCategory;
	enum class AssetType;

	constexpr uint32_t MAX_RESOURCES = 64;
	constexpr uint32_t MISSING = 0;

	//---------------------------------------------------------------------
	// ResourceAtlas
	//---------------------------------------------------------------------
	/// <summary>
	/// Centralized manager for loading, storing, and retrieving graphics resources
	/// (textures, shaders, and meshes). Provides caching to avoid duplicate loads.
	/// </summary>
	class ResourceAtlas : public core::System
	{
	public:
		/// <summary>
		/// Initializes the system, setting up necessary resources.
		/// </summary>
		/// <returns>True if the initialization was successful, otherwise false.</returns>
		bool Initialize() override;

		/// <summary>
		/// Sets the asset source used for loading assets.
		/// </summary>
		template <typename T>
		void SetAssetSource()
		{
			m_pAssetSource = std::make_shared<T>();
		}

		/// <summary>
		/// Destroys the system, releasing resources and performing necessary cleanup.
		/// </summary>
		/// <returns>True if the destruction was successful, otherwise false.</returns>
		bool Destroy() override;

		/// <summary>
		/// Loads a texture by name from the resource folder.
		/// </summary>
		std::weak_ptr<graphics::dx12::Texture> LoadTexture(const std::string& a_sName, std::shared_ptr<graphics::dx12::CommandQueue> a_pCommandQueue = nullptr);

		/// <summary>
		/// Loads a texture using a provided Direct3D resource description.
		/// </summary>
		std::weak_ptr<graphics::dx12::Texture> LoadTextureByDescription(const std::string& a_sName, D3D12_RESOURCE_DESC& a_Description);

		/// <summary>
		/// Creates an empty texture resource with the given name.
		/// </summary>
		std::weak_ptr<graphics::dx12::Texture> LoadTextureEmpty(const std::string& a_sName);

		/// <summary>
		/// Loads a pixel shader.
		/// </summary>
		std::shared_ptr<graphics::dx12::PixelShader> LoadPixelShader(const std::string& a_sName);

		/// <summary>
		/// Loads a vertex shader.
		/// </summary>
		std::shared_ptr<graphics::dx12::VertexShader> LoadVertexShader(const std::string& a_sName);

		/// <summary>
		/// Loads a scene.
		/// </summary>
		bool LoadScene(const std::string& a_sName, gameplay::Scene& a_Scene);
		
		/// <summary>
		/// Loads a mesh by name from the resource folder.
		/// </summary>
		std::weak_ptr<graphics::dx12::Mesh> LoadMesh(const std::string& a_sName, std::shared_ptr<graphics::dx12::CommandQueue> a_pCommandQueue = nullptr);

		/// <summary>
		/// Loads an empty mesh.
		/// </summary>
		std::weak_ptr<graphics::dx12::Mesh> LoadMeshEmpty(const std::string& a_sName);
		
		/// <summary>
		/// Loads a material by name from the resource folder.
		/// </summary>
		std::weak_ptr<graphics::dx12::Material> LoadMaterial(const std::string& a_sName);

		/// <summary>
		/// Creates an empty material resource with the given name.
		/// </summary>
		std::weak_ptr<graphics::dx12::Material> LoadMaterialEmpty(const std::string& a_sName);

		/// <summary>
		/// Loads a prefab.
		/// </summary>
		std::weak_ptr<gameplay::Prefab> LoadPrefab(const std::string& a_sName);

		/// <summary>
		/// Loads an animation.
		/// </summary>
		std::weak_ptr<animation::Animation> LoadAnimation(const std::string& a_sName);

		/// <summary>
		/// Retrieves the default pixel shader (used as fallback).
		/// </summary>
		std::weak_ptr<graphics::dx12::PixelShader> GetDefaultPixelShader();

		/// <summary>
		/// Retrieves the default vertex shader (used as fallback).
		/// </summary>
		std::weak_ptr<graphics::dx12::VertexShader> GetDefaultVertexShader();

		/// <summary>
		/// Retrieves the default texture (used as fallback).
		/// </summary>
		std::weak_ptr<graphics::dx12::Texture> GetDefaultTexture();

		/// <summary>
		/// Retrieves the default mesh (used as fallback).
		/// </summary>
		std::weak_ptr<graphics::dx12::Mesh> GetDefaultMesh();

		/// <summary>
		/// Retrieves the default material (used as fallback).
		/// </summary>
		std::weak_ptr<graphics::dx12::Material> GetDefaultMaterial();

		/// <summary>
		/// Gets the list of loaded textures.
		/// </summary>
		const std::vector<std::shared_ptr<graphics::dx12::Texture>>& GetTextures() const;

		/// <summary>
		/// Gets the list of loaded pixel shaders.
		/// </summary>
		const std::vector<std::shared_ptr<graphics::dx12::PixelShader>>& GetPixelShaders() const;

		/// <summary>
		/// Gets the list of loaded vertex shaders.
		/// </summary>
		const std::vector<std::shared_ptr<graphics::dx12::VertexShader>>& GetVertexShaders() const;

		/// <summary>
		/// Gets the list of loaded meshes.
		/// </summary>
		const std::vector<std::shared_ptr<graphics::dx12::Mesh>>& GetMeshes() const;

		/// <summary>
		/// Gets the list of loaded meshes.
		/// </summary>
		const std::vector<std::shared_ptr<graphics::dx12::Material>>& GetMaterials() const;

		/// <summary>
		/// Gets the list of loaded animations.
		/// </summary>
		const std::vector<std::shared_ptr<animation::Animation>>& GetAnimations() const;

		std::vector<std::weak_ptr<resources::EngineResource>> GetResourcesOfType(resources::AssetType a_AssetType, resources::EngineResourceCategory a_Category);

		std::weak_ptr<resources::IAssetSource> GetAssetSource();
	private:
		/// <summary>
		/// Retrieves a resource from the given vector, or loads it if not found.
		/// </summary>
		/// <typeparam name="T">The resource type (e.g., Texture, Shader, Mesh).</typeparam>
		/// <param name="a_vVector">Vector containing cached resources.</param>
		/// <param name="a_sName">The resource name identifier.</param>
		/// <returns>Shared pointer to the resource.</returns>
		template<class T>
		std::shared_ptr<T> GetResource(std::vector<std::shared_ptr<T>>& a_vVector, const std::string& a_sName);

		/// <summary>
		/// Checks whether a resource already exists in the given vector.
		/// </summary>
		/// <typeparam name="T">The resource type.</typeparam>
		/// <param name="a_vVector">Vector containing cached resources.</param>
		/// <param name="a_sName">The resource name identifier.</param>
		/// <returns>
		/// The index of the resource if found, otherwise -1.
		/// </returns>
		template<class T>
		int32_t HasResource(std::vector<std::shared_ptr<T>>& a_vVector, const std::string& a_sName);

		std::vector<std::shared_ptr<graphics::dx12::Texture>> m_aTextures;
		std::vector<std::shared_ptr<graphics::dx12::PixelShader>> m_aPixelShaders;
		std::vector<std::shared_ptr<graphics::dx12::VertexShader>> m_aVertexShaders;
		std::vector<std::shared_ptr<graphics::dx12::Mesh>> m_aMeshes;
		std::vector<std::shared_ptr<gameplay::Prefab>> m_aPrefabs;
		std::vector<std::shared_ptr<animation::Animation>> m_aAnimations;
		std::vector<std::shared_ptr<graphics::dx12::Material>> m_aMaterials;

		std::shared_ptr<resources::IAssetSource> m_pAssetSource;
	};
}