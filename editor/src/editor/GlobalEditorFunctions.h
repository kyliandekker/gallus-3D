#pragma once

#include <string>

// gameplay
#include "gameplay/EntityID.h"

namespace gallus::gameplay
{
	class AbstractECSSystem;
	class Component;
}
namespace gallus::editor
{
	void g_TrySetEditorScene(const std::string& a_sID);
	void g_SetEditorScene(const std::string& a_sID);
	void g_SetGameSceneToEditor();
	bool g_IsEditorSceneDirty();
	void g_SetEditorSceneDirty();
	void g_SaveEditorScene();

	gameplay::EntityID g_CreateEntity(const std::string& a_sName = "");
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