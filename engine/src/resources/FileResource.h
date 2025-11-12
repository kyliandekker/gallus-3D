#pragma once

//#ifdef _EDITOR

// external
#include <vector>
#include <string>
#include <rapidjson/document.h>
#include <unordered_map>

// resources
#include "resources/AssetType.h"
#include "resources/metadata/MetaData.h"

// utils includes
#include "utils/FILEPCH.h"

namespace gallus
{
	namespace core
	{
		class Data;
	}
	namespace resources
	{
		inline const std::unordered_map<std::string, std::vector<AssetType>> FILE_ATLAS =
		{
			{ ".scene", { AssetType::Scene } },
			{ ".prefab", { AssetType::Prefab } },
			{ ".png", { AssetType::Sprite } },
			{ ".bmp", { AssetType::Sprite } },
			{ ".wav", { AssetType::Sound, AssetType::Song, AssetType::VO } },
			{ ".anim", { AssetType::Animation } },
			{ ".hlsl", { AssetType::PixelShader, AssetType::VertexShader } },
			{ ".glb", { AssetType::Mesh } },
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
			/// Retrieves the meta data (if it exists).
			/// </summary>
			/// <returns>A pointer to the meta data.</returns>
			MetaData* GetMetaData()
			{
				return m_MetaData;
			}

			/// <summary>
			/// Retrieves the meta data (if it exists).
			/// </summary>
			/// <returns>A pointer to the meta data.</returns>
			const MetaData* GetMetaData() const
			{
				return m_MetaData;
			}

			/// <summary>
			/// Retrieves the meta data and casts it (if it exists).
			/// </summary>
			/// <returns>A pointer to the meta data.</returns>
			template<typename T>
			T* GetMetaData()
			{
				return static_cast<T*>(m_MetaData);
			}

			/// <summary>
			/// Retrieves the meta data and casts it (if it exists).
			/// </summary>
			/// <returns>A pointer to the meta data.</returns>
			template<typename T>
			const T* GetMetaData() const
			{
				return static_cast<T*>(m_MetaData);
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
		protected:
			std::vector<FileResource> m_aChildren; /// Child resources (only used for folders).
			MetaData* m_MetaData = nullptr;

			fs::path m_Path; /// The path of the resource.
			FileResource* m_Parent = nullptr; // The parent of the resource.
		};
	}
}

//#endif // _EDITOR