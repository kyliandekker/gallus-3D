#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/modals/BaseModal.h"

// external
#include <string>
#include <vector>
#include <functional>
#include <memory>

// graphics
#include "graphics/imgui/views/DataTypes/StringTextInput.h"

// resources
#include "resources/AssetType.h"

#include "editor/graphics/imgui/selectables/FileEditorSelectable.h"
#include "editor/graphics/imgui/selectables/EngineResourceEditorSelectable.h"

namespace gallus
{
	namespace resources
	{
		class FileResource;
		class EngineResource;
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
				void SetData(const std::function<void(int, const std::string&)>& a_Callback, gallus::resources::AssetType a_aFileTypes);
			private:
				std::weak_ptr<graphics::dx12::Texture> m_pPreviewTexture = {};

				std::function<void(int, const std::string&)> m_Callback = nullptr; /// The callback that gets called when choosing an option.

				EditorSelectable* m_pSelectedResource = nullptr; /// The current selected resource in the modal.
				int32_t m_iSelectedEngineResource = -1; /// The current selected resource in the modal.
				bool m_bNeedsRefresh = true; /// Used for refreshing the resources that are shown.

				resources::AssetType m_AssetType; /// The file type that will be used in the filter.
				std::vector<FileEditorSelectable> m_aResources; /// List of explorer items shown in the explorer window.
				std::vector<FileEditorSelectable*> m_aFilteredFileResources; /// List of pointers to explorer items shown in the explorer window.
				std::vector<EngineResourceEditorSelectable> m_aEngineResources; /// List of engine resources shown in the explorer window.
				std::vector<EngineResourceEditorSelectable*> m_aFilteredEngineResources; /// List of pointers to engine resources shown in the explorer window.

				SearchBarInput m_SearchBar; /// Search bar to filter specific explorer items in the explorer window.
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE