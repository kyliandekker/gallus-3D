#pragma once

#include "resources/EngineResource.h"

// external
#include <string>

// core
#include "core/Data.h"

#define JSON_SCENE_CAMERA_VAR "camera"
#define JSON_SCENE_DIRECTIONAL_LIGHT_VAR "directionalLight"

#define JSON_SCENE_ENTITIES_VAR "entities"

namespace gallus::gameplay
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
		/// Loads the scene data.
		/// </summary>
		/// <param name="a_Data">A vector containing data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data) override;

		/// <summary>
		/// Destroys the scene.
		/// </summary>
		virtual bool Destroy() override;

		/// <summary>
		/// Loads the scene data from its associated resource source.
		/// </summary>
		/// <returns>True if loading was successful, false otherwise.</returns>
		virtual bool LoadData();

		/// <summary>
		/// Returns a constant reference to the internal scene data.
		/// </summary>
		/// <returns>Reference to the scene data.</returns>
		const core::Data& GetData() const;

		/// <summary>
		/// Returns a copy of the current scene data.
		/// </summary>
		/// <returns>An object representing the scene�s data.</returns>
		virtual const core::Data GetSceneData() const;
		
		void SetSceneData(const core::Data& a_Data);

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
		
		BEGIN_SERIALIZE_PARENT(Scene, resources::EngineResource)
		END_SERIALIZE(Scene)
	};
}