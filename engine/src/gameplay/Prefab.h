#pragma once

#include "gameplay/Scene.h"

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
			bool LoadData() override;
			virtual const core::Data GetSceneData() const override;
			gameplay::EntityID Instantiate();
		};
	}
}