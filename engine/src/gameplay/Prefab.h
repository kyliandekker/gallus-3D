#pragma once

// base class
#include "gameplay/Scene.h"

// gameplay
#include "gameplay/EntityID.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Prefab
		//---------------------------------------------------------------------
		class Prefab : public Scene
		{
		public:
			/// <summary>
			/// Constructs an empty prefab.
			/// </summary>
			Prefab() = default;

			/// <summary>
			/// Constructs a prefab with a given name.
			/// </summary>
			/// <param name="a_sName">Name of the resource.</param>
			Prefab(const std::string& a_sName) : Scene(a_sName)
			{}

			/// <summary>
			/// Loads the prefab data from its associated resource source.
			/// </summary>
			/// <returns>True if loading was successful, false otherwise.</returns>
			bool LoadData() override;

#ifdef _EDITOR
			/// <summary>
			/// Returns a copy of the current prefab data.
			/// </summary>
			/// <returns>A <see cref="core::Data"/> object representing the prefab’s data.</returns>
			virtual const core::Data GetSceneData() const override;
#endif

			/// <summary>
			/// Creates a copy of the data stored in the prefab and spawns it in runtime.
			/// </summary>
			/// <returns>The newly created entity id.</returns>
			gameplay::EntityID Instantiate();
		};
	}
}