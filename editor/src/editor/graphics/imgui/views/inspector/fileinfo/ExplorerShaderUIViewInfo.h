#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ExplorerFileUIViewInfo.h"

#include <memory>

#include "graphics/imgui/views/DataTypes/StringDropdown.h"

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
			class ExplorerFileUIView;

			/// <summary>
			/// Base class for rendering shader files in the ImGui editor UI. 
			/// Provides common functionality for rendering and interacting with shader files.
			/// </summary>
			class ExplorerShaderUIViewInfo : public ExplorerFileUIViewInfo
			{
			public:
				/// <summary>
				/// Constructs the ExplorerFileUIViewInfo with an associated ImGui window 
				/// and entity ID.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_ExplorerFileUIView">The explorer file ui view.</param>
				ExplorerShaderUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView);
			protected:
				/// <summary>
				/// Renders the file UI in the editor.
				/// This function can be overridden by derived classes to render
				/// file-specific details.
				void RenderSpecific() override;

				StringDropdown<resources::AssetType> m_AssetTypeDropdown; /// Drop down UI element for selecting asset types.
			};
		}
	}
}

#include "resources/AssetType.h"

REGISTER_EXPLORER_UI((std::vector<gallus::resources::AssetType>{
	gallus::resources::AssetType::PixelShader,
	gallus::resources::AssetType::VertexShader
}), gallus::graphics::imgui::ExplorerShaderUIViewInfo);

#endif // _EDITOR
#endif // IMGUI_DISABLE