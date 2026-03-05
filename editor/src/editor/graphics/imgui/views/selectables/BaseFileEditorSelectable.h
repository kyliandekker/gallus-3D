#pragma once

// external
#include <imgui/imgui.h>
#include <string>
#include <vector>

// editor
#include "editor/graphics/imgui/views/selectables/ListItemEditorSelectable.h"

namespace gallus::resources
{
	enum class AssetType;
}
namespace gallus::file
{
	class FileResource;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	class BaseFileEditorSelectable : public ListItemEditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a file selectable.
		/// </summary>
		BaseFileEditorSelectable(ImGuiSystem& a_System, file::FileResource& a_FileResource);

		/// <summary>
		/// Sets the icon.
		/// </summary>
		void SetFileResourceProperties();

		/// <summary>
		/// Renders the element.
		/// </summary>
		void Render() override
		{}

		/// <summary>
		/// Retrieves the file resource.
		/// </summary>
		/// <returns>Reference to the file resource.</returns>
		file::FileResource& GetFileResource()
		{
			return m_FileResource;
		}

		/// <summary>
		/// Retrieves the file resource.
		/// </summary>
		/// <returns>Reference to the file resource.</returns>
		const file::FileResource& GetFileResource() const
		{
			return m_FileResource;
		}

		/// <summary>
		/// Called when the selectable gets renamed.
		/// </summary>
		/// <param name="a_sName">The new name.</param>
		void OnRename(const std::string& a_sName) override;

		/// <summary>
		/// Called when the selectable gets deleted.
		/// </summary>
		void OnDelete() override;

		/// <summary>
		/// Called when the selectable's folder gets called.
		/// </summary>
		void OnShowInExplorer() override;
	protected:
		file::FileResource& m_FileResource;
	};
}