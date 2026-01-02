#pragma once

// external
#include <string>
#ifdef _LOAD_BY_PATH
#include "utils/FILEPCH.h"
#endif // _LOAD_BY_PATH

// core
#include "core/Observable.h"

// resources
#include "resources/AssetType.h"

// editor
#include "editor/ISerializableObject.h"

namespace gallus
{
	namespace resources
	{
		enum class EngineResourceCategory
		{
			Unknown, // Default.
			Missing, // These are ESSENTIAL.
			Editor, // This is stuff like a preview texture, render target, etc.
			System, // This is more specific stuff like depth buffers, etc.
			Game // This is for any resource created by the game code.
		};

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
			virtual bool IsValid() const = 0;

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
			/// Returns whether the resource is locked.
			/// </summary>
			/// <returns>True if the resource is locked, false otherwise.</returns>
			bool IsLocked() const;

			/// <summary>
			/// Sets whether the resource is locked.
			/// </summary>
			/// <param name="a_bIsLocked">The locked state.</param>
			void SetIsLocked(bool a_bIsLocked);

			/// <summary>
			/// Returns whether the resource is unique.
			/// </summary>
			/// <returns>True if the resource is unique, false otherwise.</returns>
			bool IsUnique() const;

			/// <summary>
			/// Sets whether the resource is unique.
			/// </summary>
			/// <param name="a_bIsUnique">The unique state.</param>
			void SetIsUnique(bool a_bIsUnique);

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
			/// Returns the name of the resource.
			/// </summary>
			/// <returns>Name of the resource.</returns>
			const std::string GetName() const;

#ifdef _LOAD_BY_PATH
			/// <summary>
			/// Returns the path of the resource.
			/// </summary>
			/// <returns>Path of the resource.</returns>
			const std::filesystem::path& GetPath() const;
#endif // _LOAD_BY_PATH

			/// <summary>
			/// Loads a resource by name.
			/// </summary>
			/// <param name="a_sName">Name of the resource.</param>
			/// <returns></returns>
			virtual bool LoadByName(const std::string& a_sName);

#ifdef _LOAD_BY_PATH
			/// <summary>
			/// Loads a resource by path.
			/// </summary>
			/// <param name="a_Path">The path to the resource.</param>
			/// <returns></returns>
			virtual bool LoadByPath(const fs::path& a_Path);
#endif // _LOAD_BY_PATH

#ifdef _EDITOR
			bool IsDirty() const
			{
				return m_bIsDirty;
			}

			core::Observable<bool>& IsDirty()
			{
				return m_bIsDirty;
			}

			void SetIsDirty(bool a_bIsDirty)
			{
				m_bIsDirty = a_bIsDirty;
			}
#endif // _EDITOR
		protected:
			bool m_bIsDestroyable = true; // Whether it is destroyable once created.
			bool m_bIsLocked = false; // Whether the resource can be overridden in the atlas.
			bool m_bIsUnique = false; // Whether the resource can be shared in the atlas.

#ifdef _EDITOR
			core::Observable<bool> m_bIsDirty;
#endif // _EDITOR
			EngineResourceCategory m_ResourceCategory = EngineResourceCategory::Unknown;
			resources::AssetType m_AssetType = resources::AssetType::None;

			std::string m_sName;
#ifdef _LOAD_BY_PATH
			std::filesystem::path m_Path;
#endif // _LOAD_BY_PATH

			BEGIN_SERIALIZE(EngineResource)
			// 	SERIALIZE_FIELD(m_bIsDestroyable, "Is Destroyable", "Indicates if this resource can be destroyed or deleted.",
			// 		.type = FieldSerializationType::FieldSerializationType_Bool,
			// 		.disabled = true,
			// 		.internal = true)
			// 	SERIALIZE_FIELD(m_bIsLocked, "Is Locked", "Indicates if this resource is locked from modifications.",
			// 		.type = FieldSerializationType::FieldSerializationType_Bool,
			// 		.disabled = true,
			// 		.internal = true)
			// 	SERIALIZE_FIELD(m_bIsUnique, "Is Unique", "Specifies if this resource is unique and cannot have duplicates.",
			// 		.type = FieldSerializationType::FieldSerializationType_Bool,
			// 		.disabled = true,
			// 		.internal = true)
			// 	SERIALIZE_FIELD(m_ResourceCategory, "Resource Category", "The category this resource belongs to, used for organizational purposes.",
			// 		.type = FieldSerializationType::FieldSerializationType_Enum,
			// 		.enumToStringFunc = MakeEnumToStringFunc<EngineResourceCategory>(EngineResourceCategoryToString),
			// 		.disabled = true,
			// 		.internal = true)
			// 	SERIALIZE_FIELD(m_AssetType, "Asset Type", "The specific type of asset this resource represents.",
			// 		.type = FieldSerializationType::FieldSerializationType_Enum,
			// 		.enumToStringFunc = MakeEnumToStringFunc<AssetType>(AssetTypeToString),
			// 		.disabled = true,
			// 		.internal = true)
			END_SERIALIZE(EngineResource)
		};
	}
}