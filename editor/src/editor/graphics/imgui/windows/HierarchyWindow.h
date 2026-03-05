#pragma once

#include "imgui_system/windows/BaseWindow.h"

// external
#include <vector>

// graphics
#include "editor/graphics/imgui/views/StringTextInput.h"

// gameplay
#include "gameplay/EntityID.h"

// editor
#include "editor/graphics/imgui/views/selectables/EntityEditorSelectable.h"
#include "editor/graphics/imgui/views/selectables/CameraEditorSelectable.h"
#include "editor/graphics/imgui/views/selectables/DirectionalLightEditorSelectable.h"
#include "editor/graphics/imgui/views/Toolbar.h"

namespace gallus::resources
{
	enum class AssetType;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	//---------------------------------------------------------------------
	// HierarchyWindow
	//---------------------------------------------------------------------
	/// <summary>
	/// A window that displays and manages the entities in a scene.
	/// Allows navigation and inspection of entities in a folder-like structure.
	/// </summary>
	class HierarchyWindow : public BaseWindow
	{
	public:
		/// <summary>
		/// Constructs a hierarchy window.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		HierarchyWindow(ImGuiSystem& a_System);

		/// <summary>
		/// Cleans up and destroys the hierarchy window.
		/// </summary>
		~HierarchyWindow();

		/// <summary>
		/// Initializes all values and behaviours associated with the hierarchy window.
		/// </summary>
		/// <returns>True if initialization is successful, otherwise false.</returns>
		bool Initialize() override;

		/// <summary>
		/// Destroys and disables the hierarchy window.
		/// </summary>
		/// <returns>True if destruction is successful, otherwise false.</returns>
		bool Destroy() override;

		/// <summary>
		/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
		/// </summary>
		void Update() override;

		/// <summary>
		/// Renders the hierarchy window.
		/// </summary>
		void Render() override;
	private:
		// Toolbar.
		void PopulateToolbar();
		void DrawToolbar();

		void RegisterKeybinds();

		void DragAction(const gameplay::EntityID& a_EntityID, resources::AssetType a_AssetType, const std::string& a_sFileName);

		/// <summary>
		/// Updates all the entities shown in the hierarchy window.
		/// </summary>
		void UpdateEntities();

		/// <summary>
		/// Updates the components shown in the inspector window if applicable.
		/// </summary>
		void UpdateEntityComponents();

		void OnSelectableChanged(std::weak_ptr<EditorSelectable> a_pOldValue, std::weak_ptr<EditorSelectable> a_pNewValue);

		bool m_bNeedsRefresh = true; /// Whether the hierarchy needs to refresh the results shown in the hierarchy window.
		bool m_bSpawnEntity = false;

		std::shared_ptr<CameraEditorSelectable> m_pCameraView;
		std::shared_ptr<DirectionalLightEditorSelectable> m_pDirectionalLightView;
		
		gameplay::EntityID m_PreviousEntityID;
		std::vector<std::shared_ptr<EntityEditorSelectable>> m_aEntities; /// List of entities shown in the hierarchy window.
		std::vector<std::weak_ptr<EntityEditorSelectable>> m_aFilteredEntities; /// List of entities shown in the hierarchy window.
		std::vector<std::string> m_aEntityIcons; /// List of entities shown in the hierarchy window.

		std::string m_sSearchBarText;

		Toolbar m_Toolbar;
	};
}