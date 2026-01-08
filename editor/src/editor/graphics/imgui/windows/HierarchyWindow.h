#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

// external
#include <vector>

// graphics
#include "graphics/imgui/views/DataTypes/StringTextInput.h"

// gameplay
#include "gameplay/EntityID.h"

// editor
#include "editor/graphics/imgui/selectables/EntityEditorSelectable.h"
#include "editor/graphics/imgui/selectables/CameraEditorSelectable.h"
#include "editor/graphics/imgui/selectables/DirectionalLightEditorSelectable.h"
#include "editor/graphics/imgui/views/Toolbar.h"

namespace gallus
{
	namespace resources
	{
		enum class AssetType;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

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
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				HierarchyWindow(ImGuiWindow& a_Window);

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

				void DragAction(const gameplay::EntityID& a_EntityID, resources::AssetType a_AssetType, const std::string& a_sFileName);

				/// <summary>
				/// Updates all the entities shown in the hierarchy window.
				/// </summary>
				void UpdateEntities();

				/// <summary>
				/// Updates the components shown in the inspector window if applicable.
				/// </summary>
				void UpdateEntityComponents();

				void SetSelectable(EntityEditorSelectable* a_EntityView);

				void OnSelectableChanged(const EditorSelectable* oldVal, const EditorSelectable* newVal);

				void OnSceneDirty(const bool oldVal, const bool newVal);

				bool m_bNeedsRefresh = true; /// Whether the hierarchy needs to refresh the results shown in the hierarchy window.
				bool m_bSpawnEntity = false;

				gameplay::EntityID m_PreviousEntityID;
				std::vector<EntityEditorSelectable> m_aEntities; /// List of entities shown in the hierarchy window.
				std::vector<EntityEditorSelectable*> m_aFilteredEntities; /// List of entities shown in the hierarchy window.
				CameraEditorSelectable m_CameraView;
				DirectionalLightEditorSelectable m_DirectionalLightView;
				std::vector<std::string> m_aEntityIcons; /// List of entities shown in the hierarchy window.

				std::string m_sSearchBarText;

				Toolbar m_Toolbar;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE