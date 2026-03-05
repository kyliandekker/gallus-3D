#pragma once

#include "imgui_system/modals/BaseModal.h"

// external
#include <string>
#include <vector>
#include <functional>
#include <memory>

// graphics
#include "editor/graphics/imgui/views/StringTextInput.h"

namespace gallus::resources
{
	enum class AssetType;
	class EngineResource;
}
namespace gallus::graphics::dx12
{
	class Texture;
	class DX12Resource;
}
namespace gallus::graphics::imgui
{
	class ImGuiWindow;

	class EditorSelectable;
	class FileEditorSelectable;
	class EngineResourceEditorSelectable;

	class FilePickerModal : public BaseModal
	{
	public:
		/// <summary>
		/// Constructs a window.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		FilePickerModal(ImGuiSystem& a_System);

		void LoadTexture(const std::string& a_sName);

		/// <summary>
		/// Renders the element.
		/// </summary>
		void Render() override;

		void ListEngineResources();
		void ListFileResources();

		/// <summary>
		/// Sets the data of the file picker modal.
		/// </summary>
		/// <param name="a_Callback">Callback for modal selection.</param>
		/// <param name="a_FileTypes">The file types that should be shown in the modal.</param>
		void SetData(const std::function<void(int, const std::string&)>& a_Callback, resources::AssetType a_aFileTypes);
	private:
		std::weak_ptr<graphics::dx12::Texture> m_pPreviewTexture = {};

		std::function<void(int, const std::string&)> m_Callback = nullptr; /// The callback that gets called when choosing an option.

		std::weak_ptr<EditorSelectable> m_pSelectedResource; /// The current selected resource in the modal.
		int32_t m_iSelectedEngineResource = -1; /// The current selected resource in the modal.
		bool m_bNeedsRefresh = true; /// Used for refreshing the resources that are shown.

		resources::AssetType m_AssetType; /// The file type that will be used in the filter.

		std::vector<std::shared_ptr<FileEditorSelectable>> m_aListFileItems;
		std::vector<std::weak_ptr<FileEditorSelectable>> m_aFilteredListFileItems;
		std::vector<std::shared_ptr<EngineResourceEditorSelectable>> m_aListEngineItems;
		std::vector<std::weak_ptr<EngineResourceEditorSelectable>> m_aFilteredListEngineItems;

		SearchBarInput m_SearchBar; /// Search bar to filter specific explorer items in the explorer window.
	};
}