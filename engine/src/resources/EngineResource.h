#pragma once

// external
#include <string>

// resources
#include "resources/AssetType.h"

// resources
#include "resources/ISerializableObject.h"

namespace gallus::core
{
	class Data;
}
namespace gallus::resources
{
	enum class EngineResourceCategory
	{
		Unknown, // Default.
		Missing, // These are ESSENTIAL.
		Engine, // These are engine given resources.
		Editor, // This is stuff like a preview texture, render target, etc.
		System, // This is more specific stuff like depth buffers, etc.
		Game // This is for any resource created by the game code.
	};

#ifndef _RELEASE
	inline std::string EngineResourceCategoryToString(EngineResourceCategory resourceCategory)
	{
		switch (resourceCategory)
		{
			case EngineResourceCategory::Unknown:
			{
				return "Unknown";
			}
			case EngineResourceCategory::Missing:
			{
				return "Missing";
			}
			case EngineResourceCategory::Editor:
			{
				return "Editor";
			}
			case EngineResourceCategory::System:
			{
				return "System";
			}
			case EngineResourceCategory::Game:
			{
				return "Game";
			}
		}
		return "";
	}
#endif // _RELEASE

	//---------------------------------------------------------------------
	// EngineResource
	//---------------------------------------------------------------------
	/// <summary>
	/// Represents an engine resource with details like category, type and name.
	/// </summary>
	class EngineResource : public ISerializableObject
	{
	public:
		/// <summary>
		/// Constructs an engine resource without any details.
		/// </summary>
		EngineResource() = default;

		/// <summary>
		/// Destroys an engine resource.
		/// </summary>
		virtual bool Destroy();

		/// <summary>
		/// Returns whether the resource is a valid resource.
		/// </summary>
		/// <returns>True if the resource was valid, false otherwise.</returns>
		virtual bool IsValid() const
		{
			return false;
		}

		/// <summary>
		/// Returns whether the resource is destroyable.
		/// </summary>
		/// <returns>True if the resource is destroyable, false otherwise.</returns>
		bool IsDestroyable() const;

		/// <summary>
		/// Sets whether the resource is destroyable.
		/// </summary>
		/// <param name="a_bIsDestroyable">The destroyable state.</param>
		void SetIsDestroyable(bool a_bIsDestroyable);

		/// <summary>
		/// Sets the resource category of the engine resource.
		/// </summary>
		/// <param name="a_ResourceCategory">The resource category.</param>
		void SetResourceCategory(EngineResourceCategory a_ResourceCategory);

		/// <summary>
		/// Returns the resource category of the engine resource.
		/// </summary>
		/// <returns>Category of the resource.</returns>
		EngineResourceCategory GetResourceCategory() const;

		/// <summary>
		/// Returns the resource type of the engine resource.
		/// </summary>
		/// <returns>Type of resource.</returns>
		resources::AssetType GetAssetType() const;

		/// <summary>
		/// Sets the asset type of the engine resource.
		/// </summary>
		/// <param name="a_ResourceType">The type of asset.</param>
		void SetAssetType(resources::AssetType a_AssetType);

		/// <summary>
		/// Loads the data.
		/// </summary>
		/// <param name="a_Data">A vector containing meta data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		virtual bool LoadData(const core::Data& a_Data) = 0;

		/// <summary>
		/// Loads the meta data.
		/// </summary>
		/// <param name="a_Data">A vector containing meta data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadMetaData(const core::Data& a_Data);

		/// <summary>
		/// Returns the name of the resource.
		/// </summary>
		/// <returns>Name of the resource.</returns>
		const std::string GetName() const
		{
			return m_sName;
		}

		/// <summary>
		/// Sets the name of the resource.
		/// </summary>
		/// <param name="a_sName">The name of the resource.</param>
		void SetName(const std::string& a_sName)
		{
			m_sName = a_sName;
		}
	protected:
		bool m_bIsDestroyable = true; // Whether it is destroyable once created.

		EngineResourceCategory m_ResourceCategory = EngineResourceCategory::Unknown;
		resources::AssetType m_AssetType = resources::AssetType::None;

		std::string m_sName;

		BEGIN_SERIALIZE(EngineResource)
		 	SERIALIZE_FIELD(m_bIsDestroyable, "Is Destroyable", "Indicates if this resource can be destroyed or deleted.",
		 		.type = FieldSerializationType::FieldSerializationType_Bool,
		 		.disabled = true,
		 		.internal = true)
		 	SERIALIZE_FIELD(m_ResourceCategory, "Resource Category", "The category this resource belongs to, used for organizational purposes.",
		 		.type = FieldSerializationType::FieldSerializationType_Enum,
		 		.enumToStringFunc = MakeEnumToStringFunc<EngineResourceCategory>(EngineResourceCategoryToString),
		 		.disabled = true,
		 		.internal = true)
		 	SERIALIZE_FIELD(m_AssetType, "Asset Type", "The specific type of asset this resource represents.",
		 		.type = FieldSerializationType::FieldSerializationType_Enum,
		 		.enumToStringFunc = MakeEnumToStringFunc<AssetType>(AssetTypeToString),
		 		.disabled = true)
		END_SERIALIZE(EngineResource)
	};
}