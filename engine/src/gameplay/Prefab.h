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
			Prefab() = default;

			/// <summary>
			/// Constructs a prefab with a given name.
			/// </summary>
			/// <param name="a_sName">Name of the resource.</param>
			Prefab(const std::string& a_sName) : Scene(a_sName)
			{}

			bool LoadData() override;
			virtual const core::Data GetSceneData() const override;
			gameplay::EntityID Instantiate();
		};
	}
}