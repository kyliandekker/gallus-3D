#pragma once

// external
#include <vector>
#include <string>
#include <unordered_map>

// file
#include "file/FILEPCH.h"

// resources
#include "resources/AssetType.h"

namespace gallus::core
{
	class Data;
}
namespace gallus::file
{
	inline const std::unordered_map<std::string, std::vector<resources::AssetType>> FILE_ATLAS =
	{
		{ ".scene", { resources::AssetType::Scene } },
		{ ".prefab", { resources::AssetType::Prefab } },
		{ ".png", { resources::AssetType::Texture } },
		{ ".bmp", { resources::AssetType::Texture } },
		{ ".wav", { resources::AssetType::Sound, resources::AssetType::Song, resources::AssetType::VO } },
		{ ".anim", { resources::AssetType::Animation } },
		{ ".cso", { resources::AssetType::PixelShader, resources::AssetType::VertexShader } },
		{ ".glb", { resources::AssetType::Mesh } },
		{ ".mat", { resources::AssetType::Material } },
	};

	//---------------------------------------------------------------------
	// FileResource
	//---------------------------------------------------------------------
	class FileResource
	{
		friend class AssetDatabase;
	public:
		/// <summary>
		/// Constructs a file resource.
		/// </summary>
		FileResource();

		/// <summary>
		/// Deconstructs a file resource.
		/// </summary>
		~FileResource();

		/// <summary>
		/// Retrieves the path of the resource.
		/// </summary>
		/// <returns>The path of the resource.</returns>
		const fs::path& GetPath() const;

		/// <summary>
		/// Retrieves the name of the resource.
		/// </summary>
		/// <returns>The name of the resource.</returns>
		std::string GetName() const;

		/// <summary>
		/// Sets the path of the file resource.
		/// </summary>
		/// <param name="a_Path">The new path.</param>
		void SetPath(const fs::path& a_Path)
		{
			m_Path = a_Path;
		}

		/// <summary>
		/// Retrieves the parent.
		/// </summary>
		/// Pointer to the parent resource if that resource exists, otherwise nullptr.
		FileResource* GetParent() const;

		/// <summary>
		/// Scans the resource (and thus adding child resources if applicable).
		/// </summary>
		/// <returns>True if the scanning is successful, otherwise false.</returns>
		bool Scan();

		/// <summary>
		/// Renames the resource.
		/// </summary>
		/// <param name="a_sName">The new name (without path).</param>
		/// <returns>True if the renaming is successful, otherwise false.</returns>
		void Rename(const std::string& a_sName);

		/// <summary>
		/// Deletes the resource.
		/// </summary>
		void Delete();

		/// <summary>
		/// Searches for a path inside the file resource and children.
		/// </summary>
		/// <returns>True if the found, otherwise false.</returns>
		bool SearchForPath(const fs::path& a_Path, FileResource*& a_pFileResource);

		/// <summary>
		/// Returns the file data.
		/// </summary>
		/// <returns>True if the successful, otherwise false.</returns>
		bool GetFileData(core::Data& a_Data) const;

		/// <summary>
		/// Retrieves the children of this file resource.
		/// </summary>
		/// <returns>A vector containing all child file resources.</returns>
		std::vector<FileResource>& GetChildren()
		{
			return m_aChildren;
		}

		/// <summary>
		/// Tries and finds a file by name.
		/// </summary>
		/// <param name="a_sName">The name to search for.</param>
		/// <param name="a_pFileResource">The file resource if found.</param>
		/// <returns>True if successful, false otherwise.</returns>
		bool Find(const std::string& a_sName, FileResource*& a_pFileResource);

		/// <summary>
		/// Tries and finds a file by path.
		/// </summary>
		/// <param name="a_Path">The path to search for.</param>
		/// <param name="a_pFileResource">The file resource if found.</param>
		/// <returns>True if successful, false otherwise.</returns>
		bool Find(const fs::path& a_Path, FileResource*& a_pFileResource);

		resources::AssetType GetAssetType() const;
		void SetAssetType(resources::AssetType a_AssetType);
	protected:
		std::vector<FileResource> m_aChildren; /// Child resources (only used for folders).

		resources::AssetType m_AssetType = resources::AssetType::None;

		fs::path m_Path; /// The path of the resource.
		FileResource* m_Parent = nullptr; // The parent of the resource.
	};
}