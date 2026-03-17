#include "./GlobalEditorFunctions.h"

// core
#include "engineEntryPoints.h"

// graphics
#include "graphics/win32/Window.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/Entity.h"
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/ECSBaseSystem.h"

// resources
#include "resources/ResourceAtlas.h"

// gameplay
#include "gameplay/Game.h"

// imgui
#include "imgui_system/ImGuiSystem.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/Editor.h"
#include "editor/EditorWorkspace.h"
#include "editor/AssetDatabase.h"
#include "editor/commands/IEditorEntityCommand.h"
#include "editor/commands/IEditorEntityRenameCommand.h"
#include "editor/commands/IEditorDeleteEntityCommand.h"
#include "editor/graphics/imgui/EditorWindowsConfig.h"
#include "editor/graphics/imgui/modals/PromptModal.h"

// editor/file
#include "file/IFileAssetSource.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	void g_ShowSimplePromptModal(const std::string& a_sQuestion, const std::string& a_sLeftButtonText, const std::string& a_sRightButtonText, std::function<void()> a_ConfirmFunc)
	{
		graphics::imgui::ImGuiSystem* imguiSystem = GetEditorEngine().GetImGuiSystem();
		if (imguiSystem)
		{
			graphics::imgui::PromptModal* promptModal = imguiSystem->GetWindowsConfig<graphics::imgui::EditorWindowsConfig>().GetPromptModal();
			if (promptModal)
			{
				promptModal->SetData(a_sQuestion, {
					graphics::imgui::PromptButton{ a_sLeftButtonText, []() {} },
					graphics::imgui::PromptButton{ a_sRightButtonText, a_ConfirmFunc },
				});

				promptModal->Show();
			}
		}
	}

	//---------------------------------------------------------------------
	void g_TrySetEditorScene(const std::string& a_sID)
	{
		editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
		if (!editorWorkspace)
		{
			return;
		}

		if (!editorWorkspace->IsDirty(a_sID))
		{
			g_SetEditorScene(a_sID);
			return;
		}

		g_ShowSimplePromptModal("Do you want to ignore all unsaved actions and load a new scene?", "No", "Yes", [a_sID]()
		{
			g_SetEditorScene(a_sID);
		});
	}

	//---------------------------------------------------------------------
	void g_SetEditorScene(const std::string& a_sID)
	{
		std::shared_ptr<resources::IAssetSource> assetSrc = GetResourceAtlas().GetAssetSource().lock();
		if (!assetSrc)
		{
			return;
		}

		editor::Editor* editor = GetEditorEngine().GetEditor();
		if (!editor)
		{
			return;
		}

		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		std::lock_guard<std::recursive_mutex> lock(ecs->m_EntityMutex);

		// Reset the game scene.
		gameplay::GetGame().GetScene().Destroy();
		gameplay::GetGame().GetScene().LoadData();
		gameplay::GetGame().GetScene().SetName("");

		// First load the file data.
		core::Data data;
		if (!assetSrc->LoadBinary(a_sID, data))
		{
			LOG(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_EDITOR, "Could not load scene.");
			return;
		}
		editor->GetScene().LoadData(data);
		editor->GetScene().LoadData();
		editor->GetScene().SetName(a_sID);

		// Add it to recently loaded scenes for ease of access.
		editor->GetEditorSettings().AddSceneToRecentScenes(a_sID);
		editor->GetEditorSettings().Save();

		graphics::win32::Window* window = GetEditorEngine().GetWindow();
		if (!window)
		{
			return;
		}

		// Change window title.
		std::string title = " - (\"" + a_sID + "\")";
		window->AddTitle(title);

		editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
		if (!editorWorkspace)
		{
			return;
		}

		editorWorkspace->ClearActions();
	}

	//---------------------------------------------------------------------
	void g_SetGameSceneToEditor()
	{
		std::shared_ptr<resources::IAssetSource> assetSrc = GetResourceAtlas().GetAssetSource().lock();
		if (!assetSrc)
		{
			return;
		}

		editor::Editor* editor = GetEditorEngine().GetEditor();
		if (!editor)
		{
			return;
		}

		// First load the file data.
		core::Data data;
		assetSrc->LoadBinary(editor->GetScene().GetName(), data);
		gameplay::GetGame().GetScene().LoadData(data);
		gameplay::GetGame().GetScene().LoadData();
	}

	//---------------------------------------------------------------------
	bool g_IsEditorSceneDirty()
	{
		editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
		if (!editorWorkspace)
		{
			return false;
		}

		editor::Editor* editor = GetEditorEngine().GetEditor();
		if (!editor)
		{
			return false;
		}

		return editorWorkspace->IsDirty(editor->GetScene().GetName());
	}

	//---------------------------------------------------------------------
	void g_SetEditorSceneDirty()
	{
		editor::Editor* editor = GetEditorEngine().GetEditor();
		if (!editor)
		{
			return;
		}

		std::string name = editor->GetScene().GetName();

		// Mark scene as dirty.
		GetEditorEngine().GetEditorWorkspace()->MarkDirty(name);

		// Change window title.
		std::string title = " - (*\"" + name + "\")";
		GetEditorEngine().GetWindow()->AddTitle(title);
	}

	//---------------------------------------------------------------------
	void g_SaveEditorScene()
	{
		editor::Editor* editor = GetEditorEngine().GetEditor();
		if (!editor)
		{
			return;
		}

		std::string name = editor->GetScene().GetName();

		// Save the scene to disk.
		GetEditorEngine().GetEditorWorkspace()->Save(
			name,
			GetEditorEngine().GetEditor()->GetScene().GetSceneData(),
			false
		);

		// Change window title.
		std::string title = " - (\"" + name + "\")";
		GetEditorEngine().GetWindow()->AddTitle(title);
	}

	//---------------------------------------------------------------------
	gameplay::EntityID g_CreateEntity(const std::string& a_sName)
	{
		gameplay::EntityID entityID;

		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return entityID;
		}

		std::string newName = a_sName;
		if (newName.empty())
		{
			newName = "New GameObject";
		}
		entityID = ecs->CreateEntity(ecs->GetUniqueName(newName));

		if (entityID.IsValid())
		{
			g_SetEditorSceneDirty();
		}
		return entityID;
	}

	//---------------------------------------------------------------------
	void g_DeleteEntity(const gameplay::EntityID& a_EntityID)
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		resources::SrcData oldData;
		oldData.SetObject();
		ecs->SerializeEntity(a_EntityID, oldData);

		ecs->DeleteEntity(a_EntityID);

		std::unique_ptr<editor::IEditorDeleteEntityCommand> m_pEditorCommand = std::make_unique<editor::IEditorDeleteEntityCommand>();
		m_pEditorCommand->SetData(oldData);
		GetEditorEngine().GetEditorWorkspace()->AddAction(std::move(m_pEditorCommand));

		g_SetEditorSceneDirty();
	}

	//---------------------------------------------------------------------
	void g_SetEntityActive(const gameplay::EntityID& a_EntityID, bool a_bActive)
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		std::shared_ptr<gameplay::Entity> ent = ecs->GetEntity(a_EntityID).lock();
		if (!ent)
		{
			return;
		}

		if (ent->IsActive() == a_bActive)
		{
			return;
		}

		resources::SrcData oldData;
		oldData.SetObject();
		ecs->SerializeEntity(a_EntityID, oldData);

		ent->SetIsActive(a_bActive);

		std::unique_ptr<editor::IEditorEntityCommand> m_pEditorCommand = std::make_unique<editor::IEditorEntityCommand>();
		resources::SrcData newData;
		ecs->SerializeEntity(a_EntityID, newData);
		m_pEditorCommand->SetOldData(oldData);
		m_pEditorCommand->SetNewData(newData);
		GetEditorEngine().GetEditorWorkspace()->AddAction(std::move(m_pEditorCommand));

		g_SetEditorSceneDirty();
	}

	//---------------------------------------------------------------------
	void g_RenameEntity(const gameplay::EntityID& a_EntityID, const std::string& a_sName)
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		std::shared_ptr<gameplay::Entity> ent = ecs->GetEntity(a_EntityID).lock();
		if (!ent)
		{
			return;
		}

		std::string oldName = ent->GetName();
		ent->SetName(a_sName);

		std::unique_ptr<editor::IEditorEntityRenameCommand> m_pEditorCommand = std::make_unique<editor::IEditorEntityRenameCommand>();
		m_pEditorCommand->SetOldName(oldName);
		m_pEditorCommand->SetNewName(a_sName);
		GetEditorEngine().GetEditorWorkspace()->AddAction(std::move(m_pEditorCommand));

		g_SetEditorSceneDirty();
	}

	//---------------------------------------------------------------------
	gameplay::Component* g_CreateComponent(gameplay::AbstractECSSystem* a_pSystem, const gameplay::EntityID& a_EntityID)
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return nullptr;
		}

		resources::SrcData oldData;
		oldData.SetObject();
		ecs->SerializeEntity(a_EntityID, oldData);

		gameplay::Component* component = a_pSystem->CreateBaseComponent(a_EntityID);
		if (component)
		{
			std::unique_ptr<editor::IEditorEntityCommand> m_pEditorCommand = std::make_unique<editor::IEditorEntityCommand>();
			resources::SrcData newData;
			ecs->SerializeEntity(a_EntityID, newData);
			m_pEditorCommand->SetOldData(oldData);
			m_pEditorCommand->SetNewData(newData);
			GetEditorEngine().GetEditorWorkspace()->AddAction(std::move(m_pEditorCommand));

			g_SetEditorSceneDirty();
		}
		return component;
	}

	//---------------------------------------------------------------------
	void g_DeleteComponent(gameplay::AbstractECSSystem* a_pSystem, const gameplay::EntityID& a_EntityID)
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		resources::SrcData oldData;
		oldData.SetObject();
		ecs->SerializeEntity(a_EntityID, oldData);

		a_pSystem->DeleteComponent(a_EntityID);

		std::unique_ptr<editor::IEditorEntityCommand> m_pEditorCommand = std::make_unique<editor::IEditorEntityCommand>();
		resources::SrcData newData;
		ecs->SerializeEntity(a_EntityID, newData);
		m_pEditorCommand->SetOldData(oldData);
		m_pEditorCommand->SetNewData(newData);
		GetEditorEngine().GetEditorWorkspace()->AddAction(std::move(m_pEditorCommand));

		g_SetEditorSceneDirty();
	}
}