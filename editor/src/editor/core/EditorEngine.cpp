#include "EditorEngine.h"

// systems
#include "graphics/dx12/DX12System.h"
#include "graphics/win32/Window.h"
#include "input/InputSystem.h"
#include "resources/ResourceAtlas.h"
#include "gameplay/EntityComponentSystem.h"

// logger
#include "logger/Logger.h"

// imgui
#include "imgui_system/ImGuiSystem.h"

// editor
#include "editor/AssetDatabase.h"
#include "editor/Editor.h"
#include "editor/EditorWorkspace.h"
#include "editor/graphics/imgui/EditorWindowsConfig.h"
#include "editor/commands/IEditorCommand.h"

namespace gallus::core
{
	//---------------------------------------------------------------------
	// EditorEngine
	//---------------------------------------------------------------------
	EditorEngine::EditorEngine() : Engine::Engine()
	{
		m_pAssetDatabase = std::make_unique<editor::AssetDatabase>();
		m_pEditor = std::make_unique<editor::Editor>();
		m_pEditorWorkspace = std::make_unique<editor::EditorWorkspace>();
		m_pImGuiSystem = std::make_unique<graphics::imgui::ImGuiSystem>();
	}

	//---------------------------------------------------------------------
	EditorEngine::~EditorEngine() = default;

	//---------------------------------------------------------------------
	bool EditorEngine::Initialize(HINSTANCE a_hInstance, const std::string& a_sName)
	{
		if (!m_pAssetDatabase->Initialize(true))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_ENGINE, "Failed initializing engine: Asset database could not be initialized.");
			return false;
		}

		m_pImGuiSystem->SetupEvents();
		m_pImGuiSystem->SetWindowsConfig<graphics::imgui::EditorWindowsConfig>();
		Engine::Initialize(a_hInstance, a_sName);

		if (!m_pEditor->Initialize())
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_ENGINE, "Failed initializing engine: Editor could not be initialized.");
			return false;
		}

		m_pEditorWorkspace->Initialize();

		if (!m_pImGuiSystem->Initialize())
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_ENGINE, "Failed initializing imgui system: ImGui System could not be initialized.");
			return false;
		}

		return true;
	}

	//---------------------------------------------------------------------
	bool EditorEngine::Destroy()
	{
		m_pAssetDatabase->Destroy();

		m_pEditor->Destroy();

		return Engine::Destroy();
	}

	//---------------------------------------------------------------------
	editor::AssetDatabase* EditorEngine::GetAssetDatabase()
	{
		return m_pAssetDatabase.get();
	}

	//---------------------------------------------------------------------
	editor::Editor* EditorEngine::GetEditor()
	{
		return m_pEditor.get();
	}

	//---------------------------------------------------------------------
	editor::EditorWorkspace* EditorEngine::GetEditorWorkspace()
	{
		return m_pEditorWorkspace.get();
	}

	//---------------------------------------------------------------------
	graphics::imgui::ImGuiSystem* EditorEngine::GetImGuiSystem()
	{
		return m_pImGuiSystem.get();
	}
}