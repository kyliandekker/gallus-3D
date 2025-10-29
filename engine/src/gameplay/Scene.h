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
			Scene() = default;

			/// <summary>
			/// Constructs a scene with a given name.
			/// </summary>
			/// <param name="a_sName">Name of the resource.</param>
			Scene(const std::string& a_sName) : resources::EngineResource(a_sName)
			{}

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
			/// Loads the scene data from disk or serialized format.
			/// </summary>
			/// <returns>True if loading was successful, false otherwise.</returns>
			bool Load();
			/// <summary>
			/// Sets the internal data representation of the scene.
			/// </summary>
			/// <param name="a_Data">A <see cref="core::Data"/> object containing scene information.</param>
			void SetData(const core::Data& a_Data);

			/// <summary>
			/// Returns a constant reference to the internal scene data.
			/// </summary>
			/// <returns>Constant reference to <see cref="core::Data"/>.</returns>
			const core::Data& GetData() const;

#ifdef _EDITOR
			/// <summary>
			/// Returns a copy of the current scene data.
			/// </summary>
			/// <returns>A <see cref="core::Data"/> object representing the scene’s data.</returns>
			virtual const core::Data GetSceneData() const;

			/// <summary>
			/// Returns an observable reference indicating whether the scene has unsaved changes.
			/// </summary>
			/// <returns>Observable boolean indicating the scene’s dirty state.</returns>
			const core::Observable<bool>& IsDirty() const;
			
			/// <summary>
			/// Sets the dirty state of the scene.
			/// </summary>
			/// <param name="a_fIsDirty">True if the scene has unsaved modifications, false otherwise.</param>
			void SetIsDirty(bool a_fIsDirty);
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
			core::Observable<bool> m_fIsDirty;
#endif
		};
	}
}