#pragma once

// external
#include <rapidjson/document.h>

// resources
#include "resources/AssetType.h"

// utils
#include "utils/FILEPCH.h"

namespace gallus
{
	namespace resources
	{
		class MetaData
		{
		public:
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
			rapidjson::Document Load(const fs::path& a_Path) const;
			
			/// <summary>
			/// Generates all the metadata.
			/// </summary>
			virtual rapidjson::Document GenerateMetaData() const;
			
			/// <summary>
			/// Saves all the metadata.
			/// </summary>
			bool Save(const fs::path& a_Path) const;
			
			/// <summary>
			/// Saves all the metadata.
			/// </summary>
			virtual bool LoadMetaData(rapidjson::Document& a_Document);
			
			/// <summary>
			/// Returns whether the meta file exists.
			/// </summary>
			bool Exists(const fs::path& a_Path) const;
		protected:
			AssetType m_AssetType = AssetType::Folder; /// The asset type of the resource.
		};
	}
}