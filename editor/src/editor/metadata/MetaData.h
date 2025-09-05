#pragma once

#include <rapidjson/document.h>

// editor includes
#include "editor/AssetType.h"

namespace gallus
{
	namespace editor
	{
		class FileResource;
		
		class MetaData
		{
		public:
			MetaData(FileResource& a_FileResource);

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
			/// Loads all the metadata.
			/// </summary>
			rapidjson::Document Load() const;
			
			/// <summary>
			/// Generates all the metadata.
			/// </summary>
			virtual rapidjson::Document GenerateMetaData() const;
			
			/// <summary>
			/// Saves all the metadata.
			/// </summary>
			bool Save() const;
			
			/// <summary>
			/// Saves all the metadata.
			/// </summary>
			virtual bool LoadMetaData(rapidjson::Document& a_Document);
			
			/// <summary>
			/// Returns whether the meta file exists.
			/// </summary>
			bool Exists() const;

			FileResource& GetFileResource()
			{
				return m_FileResource;
			}
		protected:
			AssetType m_AssetType = AssetType::Folder; /// The asset type of the resource.
			
			FileResource& m_FileResource;
		};
	}
}