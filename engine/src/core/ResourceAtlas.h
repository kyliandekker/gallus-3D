#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <vector>
#include <memory>

#include "utils/file_abstractions.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
			class PixelShader;
			class VertexShader;
			class DX12ShaderBind;
			class Mesh;

			class CommandList;
		}
	}
	namespace core
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
		class ResourceAtlas
		{
		public:
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

			/// <summary>
			/// Loads a texture by name from the resource folder.
			/// </summary>
			std::shared_ptr<graphics::dx12::Texture> LoadTexture(const std::string& a_sName, std::shared_ptr<graphics::dx12::CommandList> a_pCommandList);

			/// <summary>
			/// Loads a texture using a provided Direct3D resource description.
			/// </summary>
			std::shared_ptr<graphics::dx12::Texture> LoadTextureByDescription(const std::string& a_sName, D3D12_RESOURCE_DESC& a_Description);

			/// <summary>
			/// Creates an empty texture resource with the given name.
			/// </summary>
			std::shared_ptr<graphics::dx12::Texture> LoadTextureEmpty(const std::string& a_sName);

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
			std::shared_ptr<graphics::dx12::DX12ShaderBind> LoadShaderBind(const graphics::dx12::PixelShader* a_PixelShader, const graphics::dx12::VertexShader* a_VertexShader);

			/// <summary>
			/// Loads a mesh by name from the resource folder.
			/// </summary>
			std::shared_ptr<graphics::dx12::Mesh> LoadMesh(const std::string& a_sName, std::shared_ptr<graphics::dx12::CommandList> a_pCommandList);

			/// <summary>
			/// Checks whether a mesh with the given name is already loaded.
			/// </summary>
			bool HasMesh(const std::string& a_sName);

			/// <summary>
			/// Retrieves the default pixel shader (used as fallback).
			/// </summary>
			std::shared_ptr<graphics::dx12::PixelShader> GetDefaultPixelShader();

			/// <summary>
			/// Retrieves the default vertex shader (used as fallback).
			/// </summary>
			std::shared_ptr<graphics::dx12::VertexShader> GetDefaultVertexShader();

			/// <summary>
			/// Retrieves the default shader bind (used as fallback).
			/// </summary>
			std::shared_ptr<graphics::dx12::DX12ShaderBind> GetDefaultShaderBind();

			/// <summary>
			/// Retrieves the default texture (used as fallback).
			/// </summary>
			std::shared_ptr<graphics::dx12::Texture> GetDefaultTexture();

			/// <summary>
			/// Retrieves the default mesh (used as fallback).
			/// </summary>
			std::shared_ptr<graphics::dx12::Mesh> GetDefaultMesh();

			/// <summary>
			/// Transitions all loaded resources to the appropriate state
			/// for use on the GPU.
			/// </summary>
			void TransitionResources(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList);

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
			const std::vector<std::shared_ptr<graphics::dx12::DX12ShaderBind>>& GetShaderBinds() const;

			/// <summary>
			/// Gets the list of loaded meshes.
			/// </summary>
			const std::vector<std::shared_ptr<graphics::dx12::Mesh>>& GetMeshes() const;

			/// <summary>
			/// Sets the folder used to resolve resource paths.
			/// </summary>
			void SetResourceFolder(const std::string& a_sResourceFolder)
			{
				m_sResourceFolder = a_sResourceFolder;
			}

			/// <summary>
			/// Gets the current resource folder path.
			/// </summary>
			const std::string& GetResourceFolder() const
			{
				return m_sResourceFolder;
			}
		private:
			std::vector<std::shared_ptr<graphics::dx12::Texture>> m_aTextures;
			std::vector<std::shared_ptr<graphics::dx12::PixelShader>> m_aPixelShaders;
			std::vector<std::shared_ptr<graphics::dx12::VertexShader>> m_aVertexShaders;
			std::vector<std::shared_ptr<graphics::dx12::DX12ShaderBind>> m_aShaderBinds;
			std::vector<std::shared_ptr<graphics::dx12::Mesh>> m_aMeshes;

			std::string m_sResourceFolder;
		};
	}
}