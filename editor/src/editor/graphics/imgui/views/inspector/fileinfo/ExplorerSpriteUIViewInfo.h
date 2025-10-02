#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

// base class
#include "ExplorerFileUIViewInfo.h"

// external
#include <memory>

// graphics
#include "graphics/imgui/views/DataTypes/StringDropdown.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
			enum class TextureType;
		}

		namespace imgui
		{
			class ImGuiWindow;
			class ExplorerFileUIView;

			/// <summary>
			/// Base class for rendering sprite files in the ImGui editor UI. 
			/// Provides common functionality for rendering and interacting with sprite files.
			/// </summary>
			class ExplorerSpriteUIViewInfo : public ExplorerFileUIViewInfo
			{
			public:
				/// <summary>
				/// Constructs the ExplorerFileUIViewInfo with an associated ImGui window 
				/// and entity ID.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_ExplorerFileUIView">The explorer file ui view.</param>
				ExplorerSpriteUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView);

				virtual ~ExplorerSpriteUIViewInfo();
			protected:
				/// <summary>
				/// Renders the file UI in the editor.
				/// This function can be overridden by derived classes to render
				/// file-specific details.
				void RenderSpecific() override;

				/// <summary>
				/// Renders the file UI in the editor preview section.
				/// This function can be overridden by derived classes to render
				/// file-specific details.
				void RenderPreview() override;

				int m_iCurrentSpriteIndex = 0;

				std::shared_ptr<graphics::dx12::Texture> m_pPreviewTexture = nullptr;

				StringDropdown<graphics::dx12::TextureType> m_TextureTypeDropdown; /// Drop down UI element for selecting asset types.
			};
		}
	}
}

#include "resources/AssetType.h"

REGISTER_EXPLORER_UI(std::vector<gallus::resources::AssetType>{
	gallus::resources::AssetType::Sprite
}, gallus::graphics::imgui::ExplorerSpriteUIViewInfo);

#endif // _EDITOR
#endif // IMGUI_DISABLE