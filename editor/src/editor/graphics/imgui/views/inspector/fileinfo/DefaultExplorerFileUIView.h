#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ExplorerFileUIViewInfo.h"

#include <memory>

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
			class ExplorerDefaultUIViewInfo : public ExplorerFileUIViewInfo
			{
			public:
				/// <summary>
				/// Constructs the ExplorerFileUIViewInfo with an associated ImGui window 
				/// and entity ID.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_ExplorerFileUIView">The explorer file ui view.</param>
				ExplorerDefaultUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ExplorerFileUIViewInfo(a_Window, a_ExplorerFileUIView)
				{}
			};
		}
	}
}

#include "resources/AssetType.h"

REGISTER_EXPLORER_UI(std::vector<gallus::resources::AssetType>{
	gallus::resources::AssetType::Folder
}, gallus::graphics::imgui::ExplorerDefaultUIViewInfo);

#endif // _EDITOR
#endif // IMGUI_DISABLE