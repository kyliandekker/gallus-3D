#pragma once

// external
#include <string>
#include <filesystem>

#ifdef _EDITOR
#include "editor/EditorExpose.h"
#endif

#include "core/Observable.h"

// resources
#include "resources/AssetType.h"

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

		//---------------------------------------------------------------------
		// EngineResource
		//---------------------------------------------------------------------
		/// <summary>
		/// Represents an engine resource with details like category, type and name.
		/// </summary>
		class EngineResource
#ifdef _EDITOR
			: public IExposableToEditor
#endif
		{
		public:
			/// <summary>
			/// Constructs an engine resource without any details.
			/// </summary>
			EngineResource() = default;

			/// <summary>
			/// Destroys an engine resource.
			/// </summary>
			virtual void Destroy();

			/// <summary>
			/// Constructs an engine resource with a given name.
			/// </summary>
			/// <param name="a_sName">Name of the resource.</param>
			EngineResource(const std::string& a_sName);

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
			resources::AssetType GetResourceType() const;

			/// <summary>
			/// Sets the asset type of the engine resource.
			/// </summary>
			/// <param name="a_ResourceType">The type of asset.</param>
			void SetResourceType(resources::AssetType a_AssetType);

			/// <summary>
			/// Returns the name of the resource.
			/// </summary>
			/// <returns>Name of the resource.</returns>
			const std::string GetName() const;

			/// <summary>
			/// Returns the path of the resource.
			/// </summary>
			/// <returns>Path of the resource.</returns>
			const std::filesystem::path& GetPath() const;

			/// <summary>
			/// Sets the path of the resource.
			/// </summary>
			/// <param name="a_Path">The path.</param>
			void SetPath(const std::filesystem::path& a_Path);

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
#endif
		protected:
			bool m_bIsDestroyable = true; // Whether it is destroyable once created.
			bool m_bIsLocked = false; // Whether the resource can be overridden in the atlas.
			bool m_bIsUnique = false; // Whether the resource can be shared in the atlas.

#ifdef _EDITOR
			core::Observable<bool> m_bIsDirty;
#endif
			EngineResourceCategory m_ResourceCategory = EngineResourceCategory::Unknown;
			resources::AssetType m_AssetType;

			std::string m_sName;
			std::filesystem::path m_Path;

			friend class ResourceAtlas;

#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS(EngineResource)
			END_EXPOSE_FIELDS(EngineResource)
			BEGIN_EXPOSE_GIZMOS(EngineResource)
			END_EXPOSE_GIZMOS(EngineResource)
			END_EXPOSE_TO_EDITOR(EngineResource)
#endif
		};
	}
}