#pragma once

// base class
#include "resources/EngineResource.h"

// external
#include <rapidjson/document.h>
#include <string>

// core
#include "core/Data.h"
#include "core/Observable.h"

#define JSON_SCENE_ENTITIES_VAR "entities"
#define JSON_SCENE_ENTITIES_VAR_NAME "name"
#define JSON_SCENE_ENTITIES_VAR_ACTIVE "isActive"
#define JSON_SCENE_ENTITIES_VAR_COMPONENTS "components"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Scene
		//---------------------------------------------------------------------
		/// <summary>
		/// Represents a game scene resource that holds entities and their associated data.
		/// </summary>
		class Scene : public resources::EngineResource
		{
		public:
			/// <summary>
			/// Constructs an empty scene.
			/// </summary>
			Scene() : resources::EngineResource()
			{
				m_AssetType = resources::AssetType::Scene;
			}

			/// <summary>
			/// Destroys the scene.
			/// </summary>
			virtual bool Destroy() override;

			/// <summary>
			/// Loads a resource by name.
			/// </summary>
			/// <param name="a_sName">Name of the resource.</param>
			/// <returns></returns>
			virtual bool LoadByName(const std::string& a_sName) override;

#ifdef _LOAD_BY_PATH
			/// <summary>
			/// Loads a resource by path.
			/// </summary>
			/// <param name="a_Path">The path to the resource.</param>
			/// <returns></returns>
			virtual bool LoadByPath(const fs::path& a_Path) override;
#endif

			/// <summary>
			/// Loads the scene data from its associated resource source.
			/// </summary>
			/// <returns>True if loading was successful, false otherwise.</returns>
			virtual bool LoadData();
#ifdef _EDITOR
			/// <summary>
			/// Saves the current scene data to disk or a serialized format.
			/// </summary>
			/// <returns>True if saving was successful, false otherwise.</returns>
			bool Save();
#endif
			/// <summary>
			/// Returns a constant reference to the internal scene data.
			/// </summary>
			/// <returns>Constant reference to <see cref="core::Data"/>.</returns>
			const core::Data& GetData() const;

#ifdef _EDITOR
			/// <summary>
			/// Returns a copy of the current scene data.
			/// </summary>
			/// <returns>A <see cref="core::Data"/> object representing the scene�s data.</returns>
			virtual const core::Data GetSceneData() const;
#endif
			/// <summary>
			/// Checks if the scene is in a valid state.
			/// </summary>
			/// <returns>True if the scene is valid, false otherwise.</returns>
			bool IsValid() const override
			{
				return !m_Data.empty();
			}
		protected:
			core::Data m_Data;
#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS_PARENT(Scene, resources::EngineResource)
			END_EXPOSE_FIELDS(Scene)
			BEGIN_EXPOSE_GIZMOS(Scene)
			END_EXPOSE_GIZMOS(Scene)
			END_EXPOSE_TO_EDITOR(Scene)
#endif
		};
	}
}