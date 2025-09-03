#pragma once

#ifdef _EDITOR

#include <vector>
#include <string>
#include <rapidjson/document.h>

#include "utils/file_abstractions.h"
#include "AssetType.h"

namespace gallus
{
	namespace core
	{
		class Data;
	}
	namespace editor
	{
		//---------------------------------------------------------------------
		// FileResource
		//---------------------------------------------------------------------
		class FileResource
		{
			friend class AssetDatabase;
		public:
			~FileResource();

			/// <summary>
			/// Retrieves the path of the resource.
			/// </summary>
			/// <returns>The path of the resource.</returns>
			const fs::path& GetPath() const;

			/// <summary>
			/// Retrieves the type of asset resource.
			/// </summary>
			/// <returns>AssetType containing information about what type of asset resource it is.</returns>
			AssetType GetAssetType() const;

			/// <summary>
			/// Sets the asset type.
			/// </summary>
			/// <param name="a_AssetType">The asset type it will be set to.</param>
			void SetAssetType(AssetType a_AssetType);

			/// <summary>
			/// Saves the file.
			/// </summary>
			void Save();

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
			/// Saves the metadata to its meta file.
			/// </summary>
			/// <param name="a_Document">The data to save.</param>
			/// <param name="a_Allocator">The allocator of the document.</param>
			/// <returns>True if the saving was successful, otherwise false.</returns>
			bool SaveMetadata(rapidjson::Document& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const;

			bool SearchForPath(const fs::path& a_Path, FileResource*& a_pFileResource);

			bool GetMetaData(rapidjson::Document& a_Document) const;

			bool GetFileData(core::Data& a_Data) const;

			std::vector<FileResource>& GetChildren()
			{
				return m_aChildren;
			}
		protected:
			std::vector<FileResource> m_aChildren; /// Child resources (only used for folders).

			fs::path m_Path; /// The path of the resource.
			AssetType m_AssetType = AssetType::Folder; /// The asset type of the resource.
			FileResource* m_Parent = nullptr; // The parent of the resource.
		};
	}
}

#endif // _EDITOR