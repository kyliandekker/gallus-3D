#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ExplorerFileUIViewInfo.h"

namespace gallus
{
	namespace editor
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
			/// Base class for rendering Animation files in the ImGui editor UI. 
			/// Provides common functionality for rendering and interacting with Animation files.
			/// </summary>
			class ExplorerAnimationUIViewInfo : public ExplorerFileUIViewInfo
			{
			public:
				/// <summary>
				/// Constructs the ExplorerFileUIViewInfo with an associated ImGui window 
				/// and entity ID.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_ExplorerFileUIView">The explorer file ui view.</param>
				ExplorerAnimationUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView);
			protected:
				/// <summary>
				/// Renders the file UI in the editor.
				/// This function can be overridden by derived classes to render
				/// file-specific details.
				void RenderSpecific() override;
			};
		}
	}
}

#include "resources/AssetType.h"

REGISTER_EXPLORER_UI(std::vector<gallus::resources::AssetType>{
	gallus::resources::AssetType::Animation
}, gallus::graphics::imgui::ExplorerAnimationUIViewInfo);

#endif // _EDITOR
#endif // IMGUI_DISABLE