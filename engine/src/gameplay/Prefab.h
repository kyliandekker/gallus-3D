#pragma once

#include "gameplay/Scene.h"

// gameplay
#include "gameplay/EntityID.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// Prefab
	//---------------------------------------------------------------------
	class Prefab : public Scene
	{
	public:
		/// <summary>
		/// Loads the prefab data.
		/// </summary>
		/// <param name="a_Data">A vector containing data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data) override;

		/// <summary>
		/// Loads the prefab data from its associated resource source.
		/// </summary>
		/// <returns>True if loading was successful, false otherwise.</returns>
		bool LoadData() override;

		/// <summary>
		/// Returns a copy of the current prefab data.
		/// </summary>
		/// <returns>An object representing the prefab's data.</returns>
		virtual const core::Data GetSceneData() const override;

		/// <summary>
		/// Creates a copy of the data stored in the prefab and spawns it in runtime.
		/// </summary>
		/// <returns>The newly created entity id.</returns>
		gameplay::EntityID Instantiate() const;
	};
}