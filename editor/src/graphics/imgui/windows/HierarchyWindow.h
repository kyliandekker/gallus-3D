#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

#include <vector>

#include "gameplay/EntityID.h"
#include "graphics/imgui/views/DataTypes/StringTextInput.h"
#include "graphics/imgui/views/HierarchyEntityUIView.h"

namespace gallus
{
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
				/// Renders the hierarchy window.
				/// </summary>
				void Render() override;
			private:
				/// <summary>
				/// Updates all the entities shown in the hierarchy window.
				/// </summary>
				void UpdateEntities();

				/// <summary>
				/// Updates the components shown in the inspector window if applicable.
				/// </summary>
				void UpdateEntityComponents();

				void SetSelectable(HierarchyEntityUIView* a_EntityView);

				void OnSelectableChanged(const EditorSelectable* oldVal, const EditorSelectable* newVal);

				bool m_bNeedsRefresh = true; /// Whether the hierarchy needs to refresh the results shown in the hierarchy window.

				gameplay::EntityID m_PreviousEntityID;
				std::vector<HierarchyEntityUIView> m_aEntities; /// List of entities shown in the hierarchy window.
				std::vector<HierarchyEntityUIView*> m_aFilteredEntities; /// List of entities shown in the hierarchy window.
				std::vector<std::string> m_aEntityIcons; /// List of entities shown in the hierarchy window.

				SearchBarInput m_SearchBar; /// Search bar to filter specific messages in the hierarchy window.
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE