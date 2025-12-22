#pragma once

#include "core/System.h"

// external
#include <vector>
#include <memory>

// utils
#include "utils/FILEPCH.h"

#include "graphics/dx12/DX12PCH.h"

// resources
#include "resources/FileResource.h"

struct D3D12_RESOURCE_DESC;

namespace gallus
{
	namespace resources
	{
		class EngineResource;
		enum class EngineResourceCategory;
	}
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
			class PixelShader;
			class VertexShader;
			class ShaderBind;
			class Mesh;
			class Material;

			class CommandQueue;
			class CommandList;
		}
	}
	namespace animation
	{
		class Animation;
	}
	namespace gameplay
	{
		class Scene;
		class Prefab;
	}
	namespace resources
	{
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
			/// Checks whether a texture with the given name is already loaded.
			/// </summary>
			bool HasTexture(const std::string& a_sName);

			/// <summary>
			/// Loads a pixel shader.
			/// </summary>
			std::shared_ptr<graphics::dx12::PixelShader> LoadPixelShader(const std::string& a_sName);

			/// <summary>
			/// Loads a vertex shader.
			/// </summary>
			std::shared_ptr<graphics::dx12::VertexShader> LoadVertexShader(const std::string& a_sName);

			/// <summary>
			/// Checks whether a pixel shader with the given name is already loaded.
			/// </summary>
			bool HasPixelShader(const std::string& a_sName);

			/// <summary>
			/// Checks whether a vertex shader with the given name is already loaded.
			/// </summary>
			bool HasVertexShader(const std::string& a_sName);

			/// <summary>
			/// Loads a shader bind.
			/// </summary>
			std::weak_ptr<graphics::dx12::ShaderBind> LoadShaderBind(const std::string& a_sName, std::shared_ptr<graphics::dx12::PixelShader> a_PixelShader, std::shared_ptr<graphics::dx12::VertexShader> a_VertexShader, CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT a_DSVFormat = DXGI_FORMAT_D32_FLOAT);

			/// <summary>
			/// Loads a shader bind.
			/// </summary>
			std::weak_ptr<graphics::dx12::ShaderBind> LoadShaderBind(const std::string& a_sName);

			/// <summary>
			/// Loads a scene.
			/// </summary>
			bool LoadScene(const std::string& a_sName, gameplay::Scene& a_Scene);

			/// <summary>
			/// Loads an empty mesh.
			/// </summary>
			std::weak_ptr<graphics::dx12::Mesh> LoadMeshEmpty(const std::string& a_sName);

			/// <summary>
			/// Loads a mesh by name from the resource folder.
			/// </summary>
			std::weak_ptr<graphics::dx12::Mesh> LoadMesh(const std::string& a_sName, std::shared_ptr<graphics::dx12::CommandQueue> a_pCommandQueue = nullptr);

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
			/// Checks whether a mesh with the given name is already loaded.
			/// </summary>
			bool HasMesh(const std::string& a_sName);

			/// <summary>
			/// Retrieves the default pixel shader (used as fallback).
			/// </summary>
			std::weak_ptr<graphics::dx12::PixelShader> GetDefaultPixelShader();

			/// <summary>
			/// Retrieves the default vertex shader (used as fallback).
			/// </summary>
			std::weak_ptr<graphics::dx12::VertexShader> GetDefaultVertexShader();

			/// <summary>
			/// Retrieves the default shader bind (used as fallback).
			/// </summary>
			std::weak_ptr<graphics::dx12::ShaderBind> GetDefaultShaderBind();

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
			/// Gets the list of shader binds.
			/// </summary>
			const std::vector<std::shared_ptr<graphics::dx12::ShaderBind>>& GetShaderBinds() const;

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

//#ifdef _EDITOR
			/// <summary>
			/// Gets a resource (if it exists) by name and returns the full path and all properties, including metadata.
			/// </summary>
			bool GetResource(const std::string& a_sName, resources::FileResource*& a_pResource)
			{
				return m_ResourceFolder.Find(a_sName, a_pResource);
			}

			/// <summary>
			/// Gets a resource (if it exists) by path and returns the full path and all properties, including metadata.
			/// </summary>
			bool GetResource(const fs::path& a_Path, resources::FileResource*& a_pResource)
			{
				return m_ResourceFolder.Find(a_Path, a_pResource);
			}

			/// <summary>
			/// Retrieves the current resource folder.
			/// </summary>
			resources::FileResource& GetResourceFolder()
			{
				return m_ResourceFolder;
			}

			/// <summary>
			/// Retrieves the current resource folder.
			/// </summary>
			const resources::FileResource& GetResourceFolder() const
			{
				return m_ResourceFolder;
			}
//#endif _EDITOR // _EDITOR
		private:
			/// <summary>
			/// Retrieves a resource from the given vector, or loads it if not found.
			/// </summary>
			/// <typeparam name="T">The resource type (e.g., Texture, Shader, Mesh).</typeparam>
			/// <param name="a_vVector">Vector containing cached resources.</param>
			/// <param name="a_sName">The resource name identifier.</param>
			/// <param name="a_Path">The filesystem path to the resource.</param>
			/// <returns>Shared pointer to the resource.</returns>
			template<class T>
			std::shared_ptr<T> GetResource(std::vector<std::shared_ptr<T>>& a_vVector, const std::string& a_sName, const fs::path& a_Path);

			/// <summary>
			/// Checks whether a resource already exists in the given vector.
			/// </summary>
			/// <typeparam name="T">The resource type.</typeparam>
			/// <param name="a_vVector">Vector containing cached resources.</param>
			/// <param name="a_sName">The resource name identifier.</param>
			/// <param name="a_Path">The filesystem path to the resource.</param>
			/// <returns>
			/// The index of the resource if found, otherwise -1.
			/// </returns>
			template<class T>
			int32_t HasResource(std::vector<std::shared_ptr<T>>& a_vVector, const std::string& a_sName, const fs::path& a_Path);

			std::vector<std::shared_ptr<graphics::dx12::Texture>> m_aTextures;
			std::vector<std::shared_ptr<graphics::dx12::PixelShader>> m_aPixelShaders;
			std::vector<std::shared_ptr<graphics::dx12::VertexShader>> m_aVertexShaders;
			std::vector<std::shared_ptr<graphics::dx12::ShaderBind>> m_aShaderBinds;
			std::vector<std::shared_ptr<graphics::dx12::Mesh>> m_aMeshes;
			std::vector<std::shared_ptr<gameplay::Prefab>> m_aPrefabs;
			std::vector<std::shared_ptr<animation::Animation>> m_aAnimations;
			std::vector<std::shared_ptr<graphics::dx12::Material>> m_aMaterials;

			//#ifdef _EDITOR
			resources::FileResource m_ResourceFolder;
			//#endif _EDITOR // _EDITOR
		};
	}
}