#pragma once

#include <rapidjson/document.h>

// utils includes
#include "utils/file_abstractions.h"

// resources includes
#include "resources/AssetType.h"

namespace gallus
{
	namespace resources
	{
		class MetaData
		{
		public:
			/// <summary>
			/// Constructs meta data.
			/// </summary>
			MetaData();

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
			/// <returns>A document containing all meta data.</returns>
			rapidjson::Document Load(const fs::path& a_Path) const;
			
			/// <summary>
			/// Generates all the metadata.
			/// </summary>
			/// <returns>A document containing all meta data.</returns>
			virtual rapidjson::Document GenerateMetaData() const;
			
			/// <summary>
			/// Saves all the metadata.
			/// </summary>
			/// <param name="a_Path">The path to save to.</param>
			/// <returns>True if saving was successful, false otherwise.</returns>
			bool Save(const fs::path& a_Path) const;
			
			/// <summary>
			/// Saves all the metadata.
			/// </summary>
			/// <param name="a_Document">The document to store the metadata in.</param>
			/// <returns>True if loading was successful, false otherwise.</returns>
			virtual bool LoadMetaData(rapidjson::Document& a_Document);
			
			/// <summary>
			/// Returns whether the meta file exists.
			/// </summary>
			/// <param name="a_Path">The path to check.</param>
			/// <returns>True if file existed, false otherwise.</returns>
			bool Exists(const fs::path& a_Path) const;
		protected:
			AssetType m_AssetType = AssetType::Folder; /// The asset type of the resource.
		};
	}
}