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

		void g_SaveScene();
		void g_SetScene(const fs::path& a_Path, editor::EditorMethod a_EditorMethod);

		gameplay::EntityID g_CreateEntity(const std::string& a_sName = "");
		gameplay::EntityID g_InstantiatePrefab(const gameplay::Prefab& a_Prefab);
		void g_DeleteEntity(const gameplay::EntityID& a_EntityID);
		void g_SetEntityActive(const gameplay::EntityID& a_EntityID, bool a_bActive);
		void g_RenameEntity(const gameplay::EntityID& a_EntityID, const std::string& a_sName);

		gameplay::Component* g_CreateComponent(gameplay::AbstractECSSystem* a_pSystem, const gameplay::EntityID& a_EntityID);
		template<typename ComponentType>
		ComponentType* g_CreateComponentOfType(gameplay::AbstractECSSystem* a_pSystem, const gameplay::EntityID& a_EntityID)
		{
			return dynamic_cast<ComponentType*>(g_CreateComponent(a_pSystem, a_EntityID));
		}
		void g_DeleteComponent(gameplay::AbstractECSSystem* a_pSystem, const gameplay::EntityID& a_EntityID);
	}
}