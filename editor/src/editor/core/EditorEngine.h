#pragma once

// core
#include "core/Engine.h"

namespace gallus::core
{
	class EditorEngine;
}
namespace gallus::editor
{
	class AssetDatabase;
	class Editor;
	class EditorWorkspace;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;
}
namespace gallus
{
	class EditorEngine;
	extern core::EditorEngine& GetEditorEngine();
}
namespace gallus::core
{
	//---------------------------------------------------------------------
	// EditorEngine
	//---------------------------------------------------------------------
	class EditorEngine : public Engine
	{
	public:
		EditorEngine();

		~EditorEngine();

		/// <summary>
		/// Initializes the engine and all necessary subsystems with the specified parameters.
		/// </summary>
		/// <param name="a_hInstance">Handle to the program instance.</param>
		/// <param name="a_sName">Name of the program and window.</param>
		/// <returns>True if the engine initializes successfully, otherwise false.</returns>
		bool Initialize(HINSTANCE a_hInstance, const std::string& a_sName) override;

		/// <summary>
		/// Shuts down the engine and cleans up all subsystems.
		/// </summary>
		/// <returns>True if the destruction is successful, otherwise false.</returns>
		bool Destroy() override;

		/// <summary>
		/// Retrieves the asset database.
		/// </summary>
		/// <returns>Pointer to the asset database.</returns>
		editor::AssetDatabase* GetAssetDatabase();

		/// <summary>
		/// Retrieves the editor.
		/// </summary>
		/// <returns>Pointer to the editor.</returns>
		editor::Editor* GetEditor();

		/// <summary>
		/// Retrieves the editor workspace.
		/// </summary>
		/// <returns>Pointer to the editor workspace.</returns>
		editor::EditorWorkspace* GetEditorWorkspace();

		/// <summary>
		/// Retrieves the ImGui System.
		/// </summary>
		/// <returns>Pointer to the ImGui System.</returns>
		graphics::imgui::ImGuiSystem* GetImGuiSystem();
	private:
		std::unique_ptr<editor::AssetDatabase> m_pAssetDatabase;
		std::unique_ptr<editor::Editor> m_pEditor;
		std::unique_ptr<editor::EditorWorkspace> m_pEditorWorkspace;
		std::unique_ptr<graphics::imgui::ImGuiSystem> m_pImGuiSystem;
	};
}