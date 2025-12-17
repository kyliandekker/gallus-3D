#pragma once

#include "utils/FILEPCH.h"

#include "gameplay/EntityID.h"

namespace gallus
{
	namespace gameplay
	{
		class AbstractECSSystem;
		class Component;

		class Prefab;
	}
	namespace editor
	{
		enum class EditorMethod;

		void G_SaveScene();
		void G_SetScene(const fs::path& a_Path, editor::EditorMethod a_EditorMethod);

		gameplay::EntityID G_CreateEntity(const std::string& a_sName = "");
		gameplay::EntityID G_InstantiatePrefab(const gameplay::Prefab& a_Prefab);
		void G_DeleteEntity(const gameplay::EntityID& a_EntityID);
		void G_SetEntityActive(const gameplay::EntityID& a_EntityID, bool a_bActive);
		void G_RenameEntity(const gameplay::EntityID& a_EntityID, const std::string& a_sName);

		gameplay::Component* G_CreateComponent(gameplay::AbstractECSSystem* a_pSystem, const gameplay::EntityID& a_EntityID);
		template<typename ComponentType>
		ComponentType* G_CreateComponentOfType(gameplay::AbstractECSSystem* a_pSystem, const gameplay::EntityID& a_EntityID)
		{
			return dynamic_cast<ComponentType*>(G_CreateComponent(a_pSystem, a_EntityID));
		}
		void G_DeleteComponent(gameplay::AbstractECSSystem* a_pSystem, const gameplay::EntityID& a_EntityID);
	}
}