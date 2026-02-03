#include "EditorGlobalFunctions.h"

// editor
#include "editor/core/EditorEngine.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/ECSBaseSystem.h"
#include "gameplay/Prefab.h"

// resources
#include "resources/ResourceAtlas.h"

namespace gallus
{
	namespace editor
	{
		void g_SaveScene()
		{
			resources::ResourceAtlas* resourceAtlas = core::ENGINE->GetResourceAtlas();
			if (!resourceAtlas)
			{
				return;
			}

			if (!core::EDITOR_ENGINE->GetEditor().GetScene().IsValid())
			{
				fs::path scenePath;
				if (file::SaveFile(scenePath, {
					{ L"Scene Files (*.scene)", L"*.scene" },
					{ L"Prefab Files (*.prefab)", L"*.prefab" },
					}, resourceAtlas->GetResourceFolder().GetPath()))
				{
					if (scenePath.extension() == ".scene")
					{
						core::EDITOR_ENGINE->GetEditor().SetEditorMethod(editor::EditorMethod::EDITOR_METHOD_SCENE);
					}
					else if (scenePath.extension() == ".prefab")
					{
						core::EDITOR_ENGINE->GetEditor().SetEditorMethod(editor::EditorMethod::EDITOR_METHOD_PREFAB);
					}

					core::EDITOR_ENGINE->GetEditor().GetScene().Save(scenePath);
				}
			}
			else
			{
				core::EDITOR_ENGINE->GetEditor().GetScene().Save();
			}
		}

		void g_SetScene(const fs::path& a_Path, editor::EditorMethod a_EditorMethod)
		{
			core::EDITOR_ENGINE->GetEditor().SetEditorMethod(a_EditorMethod);

			core::EDITOR_ENGINE->GetEditor().GetScene().LoadByPath(a_Path);
			core::EDITOR_ENGINE->GetEditor().GetScene().LoadData();

			core::EDITOR_ENGINE->GetEditor().GetEditorSettings().AddSceneToRecentScenes(a_Path.generic_string());
			core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
		}

		gameplay::EntityID g_CreateEntity(const std::string& a_sName)
		{
			gameplay::EntityID entityID;

			gameplay::EntityComponentSystem* ecs = core::ENGINE->GetECS();
			if (!ecs)
			{
				return entityID;
			}

			if (a_sName.empty())
			{
				entityID = ecs->CreateEntity(ecs->GetUniqueName("New GameObject"));
			}
			else
			{
				entityID = ecs->CreateEntity(ecs->GetUniqueName(a_sName));
			}

			core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
			return entityID;
		}

		gameplay::EntityID g_InstantiatePrefab(const gameplay::Prefab& a_Prefab)
		{
			gameplay::EntityID entity = a_Prefab.Instantiate();
			core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);

			return entity;
		}

		void g_DeleteEntity(const gameplay::EntityID& a_EntityID)
		{
			gameplay::EntityComponentSystem* ecs = core::ENGINE->GetECS();
			if (!ecs)
			{
				return;
			}

			ecs->DeleteEntity(a_EntityID);
			core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
		}

		void g_SetEntityActive(const gameplay::EntityID& a_EntityID, bool a_bActive)
		{
			gameplay::EntityComponentSystem* ecs = core::ENGINE->GetECS();
			if (!ecs)
			{
				return;
			}

			auto ent = ecs->GetEntity(a_EntityID).lock();
			if (!ent)
			{
				return;
			}

			if (ent->IsActive() == a_bActive)
			{
				return;
			}

			ent->SetIsActive(a_bActive);
			core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
		}

		void g_RenameEntity(const gameplay::EntityID& a_EntityID, const std::string& a_sName)
		{
			gameplay::EntityComponentSystem* ecs = core::ENGINE->GetECS();
			if (!ecs)
			{
				return;
			}

			auto ent = ecs->GetEntity(a_EntityID).lock();
			if (!ent)
			{
				return;
			}

			ent->SetName(a_sName);
			core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
		}

		gameplay::Component* g_CreateComponent(gameplay::AbstractECSSystem* a_pSystem, const gameplay::EntityID& a_EntityID)
		{
			gameplay::Component* component = a_pSystem->CreateBaseComponent(a_EntityID);
			core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
			return component;
		}

		void g_DeleteComponent(gameplay::AbstractECSSystem* a_pSystem, const gameplay::EntityID& a_EntityID)
		{
			a_pSystem->DeleteComponent(a_EntityID);
			core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
		}
	}
}
