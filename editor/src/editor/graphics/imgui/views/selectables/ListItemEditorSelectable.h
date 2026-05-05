#pragma once

// external
#include <imgui/imgui.h>
#include <string>
#include <vector>

// editor
#include "editor/graphics/imgui/views/selectables/EditorSelectable.h"

namespace gallus::resources
{
	enum class AssetType;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	std::string AssetTypeToFileIcon(resources::AssetType a_AssetType);

	ImVec4 Shade(const ImVec4& color, float factor);

	ImVec4 ShiftWithinFamily(const ImVec4& base, float slot, float totalSlots);

	ImVec4 AssetTypeToColor(resources::AssetType a_AssetType);

	class ListItemEditorSelectable : public EditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a list item selectable.
		/// </summary>
		ListItemEditorSelectable(ImGuiSystem& a_System);

		/// <summary>
		/// Sets the icon string for the file resource.
		/// </summary>
		void SetIcon(const std::string& a_sIcon);

		/// <summary>
		/// Renders the file resource in the UI.
		/// </summary>
		/// <param name="a_bSelected">Indicates if the resource is selected.</param>
		void RenderList(bool a_bSelected);

		/// <summary>
		/// Renders the element.
		/// </summary>
		void Render() override
		{}

		/// <summary>
		/// Sets the name of the file selectable.
		/// </summary>
		/// <param name="a_sName">The new name.</param>
		void SetName(const std::string& a_sName)
		{
			m_sName = a_sName;
		}

		/// <summary>
		/// Returns the name of the selectable.
		/// </summary>
		/// <returns>String containing the name.</returns>
		const std::string& GetName() const override;

		/// <summary>
		/// Retrieves the icon string for the resource.
		/// </summary>
		/// <returns>A reference to the icon string.</returns>
		const std::string& GetIcon() const override;
	protected:
		std::string m_sName; /// The name that will be displayed in imgui.
		std::string m_sIcon; /// The icon of the resource.
		resources::AssetType m_AssetType; /// The asset type of the resource.
	};
}