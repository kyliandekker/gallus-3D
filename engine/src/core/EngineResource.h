#pragma once

#include <string>
#include <filesystem>

namespace gallus
{
	namespace core
	{
		enum class EngineResourceCategory
		{
			Unknown, // Default.
			Missing, // These are ESSENTIAL.
			Editor, // This is stuff like a preview texture, render target, etc.
			System, // This is more specific stuff like depth buffers, etc.
			Game // This is for any resource created by the game code.
		};

		enum class ResourceType
		{
			ResourceType_Unknown, // THIS SHOULD NEVER HAPPEN IF ONE IS ALLOCATED.
			ResourceType_Texture,
			ResourceType_Shader,
			ResourceType_Mesh,
			ResourceType_Material,
		};

		std::string ResourceTypeToString(ResourceType a_ResourceType);

		//---------------------------------------------------------------------
		// EngineResource
		//---------------------------------------------------------------------
		/// <summary>
		/// Represents an engine resource with details like category, type and name.
		/// </summary>
		class EngineResource
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
			ResourceType GetResourceType() const;

			/// <summary>
			/// Sets the resource type of the engine resource.
			/// </summary>
			/// <param name="a_ResourceType">The type of resource.</param>
			void SetResourceType(ResourceType a_ResourceType);

			/// <summary>
			/// Returns the name of the resource.
			/// </summary>
			/// <returns>Name of the resource.</returns>
			const std::string& GetName() const;

			/// <summary>
			/// Returns the path of the resource.
			/// </summary>
			/// <returns>Path of the resource.</returns>
			const std::filesystem::path& GetPath() const;
		protected:
			bool m_bIsDestroyable = true; // Whether it is destroyable once created.
			bool m_bIsLocked = false; // Whether the resource can be overridden in the atlas.
			bool m_bIsUnique = false; // Whether the resource can be shared in the atlas.

			EngineResourceCategory m_ResourceCategory = EngineResourceCategory::Unknown;
			ResourceType m_ResourceType = ResourceType::ResourceType_Unknown;

			std::string m_sName;
			std::filesystem::path m_Path;

			friend class ResourceAtlas;
		};
	}
}