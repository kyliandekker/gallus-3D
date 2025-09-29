#pragma once

#ifdef _EDITOR

#include <vector>
#include <string>
#include <rapidjson/document.h>
#include <unordered_map>

// utils includes
#include "utils/file_abstractions.h"

// editor includes
#include "resources/AssetType.h"
#include "resources/metadata/MetaData.h"

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
			FileResource();

			~FileResource();

			/// <summary>
			/// Retrieves the path of the resource.
			/// </summary>
			/// <returns>The path of the resource.</returns>
			const fs::path& GetPath() const;

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

			std::vector<FileResource>& GetChildren()
			{
				return m_aChildren;
			}

			MetaData* GetMetaData()
			{
				return m_MetaData;
			}

			const MetaData* GetMetaData() const
			{
				return m_MetaData;
			}

			template<typename T>
			T* GetMetaData()
			{
				return static_cast<T*>(m_MetaData);
			}

			template<typename T>
			const T* GetMetaData() const
			{
				return static_cast<T*>(m_MetaData);
			}

			bool Find(const std::string& a_sName, FileResource*& a_pFileResource);
			bool Find(const fs::path& a_Path, FileResource*& a_pFileResource);
		protected:
			std::vector<FileResource> m_aChildren; /// Child resources (only used for folders).
			MetaData* m_MetaData = nullptr;

			fs::path m_Path; /// The path of the resource.
			FileResource* m_Parent = nullptr; // The parent of the resource.
		};
	}
}

#endif // _EDITOR