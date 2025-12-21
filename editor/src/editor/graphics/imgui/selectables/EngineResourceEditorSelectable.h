#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

// external
#include <imgui/imgui.h>
#include <string>
#include <vector>

// utils
#include "utils/file_abstractions.h"

// editor
#include "editor/graphics/imgui/EditorSelectable.h"

namespace gallus
{
	namespace resources
	{
		class EngineResource;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;
			class FileEditorSelectables;

			class EngineResourceEditorSelectable : public EditorSelectable
			{
			public:
				EngineResourceEditorSelectable(ImGuiWindow& a_Window, std::weak_ptr<resources::EngineResource> a_pResource);

				/// <summary>
				/// Retrieves the icon string for the resource.
				/// </summary>
				/// <returns>A reference to the icon string.</returns>
				std::string GetIcon() const override;

				void SetIcon();

				/// <summary>
				/// Sets the icon string for the resource.
				/// </summary>
				void SetIcon(const std::string& a_sIcon);

				/// <summary>
				/// Renders the file resource in the UI.
				/// </summary>
				/// <param name="a_bClicked">Indicates if the resource was clicked.</param>
				/// <param name="a_bRightClicked">Indicates if the resource was right-clicked.</param>
				/// <param name="a_bDoubleClicked">Indicates if the resource was double-clicked.</param>
				/// <param name="a_bSelected">Indicates if the resource is selected.</param>
				/// <param name="a_bInContextMenu">Indicates if the resource is currently open in the context menu.</param>
				void RenderList(bool& a_bClicked, bool& a_bRightClicked, bool& a_bDoubleClicked, bool a_bSelected, bool a_bInContextMenu);

				void Render() override
				{
				}

				std::weak_ptr<resources::EngineResource> GetEngineResource()
				{
					return m_pResource;
				}

				const std::weak_ptr<resources::EngineResource> GetEngineResource() const
				{
					return m_pResource;
				}

				void SetDisplayName(const std::string& a_sDisplayName)
				{
					m_sDisplayName = a_sDisplayName;
				}
			private:
				std::string m_sDisplayName; /// The name that will be displayed in imgui.
				std::string m_sIcon; /// The icon of the resource.
				std::weak_ptr<resources::EngineResource> m_pResource = {};
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE