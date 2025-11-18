#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/modals/BaseModal.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "graphics/imgui/views/DataTypes/StringTextInput.h"
#include "resources/AssetType.h"

namespace gallus
{
	namespace resources
	{
		class FileResource;
	}
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
			class DX12Resource;
		}

		namespace imgui
		{
			class ImGuiWindow;

			class FileEditorSelectable;

			class FilePickerModal : public BaseModal
			{
			public:
				/// <summary>
				/// Constructs a window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				FilePickerModal(ImGuiWindow& a_Window);

				void LoadTexture(const std::string& a_sName);

				/// <summary>
				/// Renders the element.
				/// </summary>
				void Render() override;

				void Show() override;

				/// <summary>
				/// Sets the data of the file picker modal.
				/// </summary>
				/// <param name="a_Callback">Callback for modal selection.</param>
				/// <param name="a_FileTypes">The file types that should be shown in the modal.</param>
				void SetData(const std::function<void(int, gallus::resources::FileResource&)>& a_Callback, const std::vector<gallus::resources::AssetType>& a_aFileTypes);
			private:
				std::shared_ptr<graphics::dx12::Texture> m_pPreviewTexture = nullptr;

				std::function<void(int, gallus::resources::FileResource&)> m_Callback = nullptr; /// The callback that gets called when choosing an option.

				FileEditorSelectable* m_pSelectedFileResource = nullptr; /// The current selected resource in the modal.
				std::shared_ptr<graphics::dx12::DX12Resource> m_pSelectedResource = nullptr; /// The current selected resource in the modal.
				bool m_bNeedsRefresh = true; /// Used for refreshing the resources that are shown.

				std::vector<gallus::resources::AssetType> m_aFileTypes; /// The file types that will be used in the filter.
				std::vector<FileEditorSelectable> m_aResources; /// List of explorer items shown in the explorer window.
				std::vector<FileEditorSelectable*> m_aFilteredFileResources; /// List of explorer items shown in the explorer window.

				SearchBarInput m_SearchBar; /// Search bar to filter specific explorer items in the explorer window.
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE