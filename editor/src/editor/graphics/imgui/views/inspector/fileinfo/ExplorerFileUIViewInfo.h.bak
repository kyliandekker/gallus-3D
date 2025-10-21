#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;
			class ExplorerFileUIView;

			/// <summary>
			/// Base class for rendering files in the ImGui editor UI. 
			/// Provides common functionality for rendering and interacting with files.
			/// </summary>
			class ExplorerFileUIViewInfo : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs the ExplorerFileUIViewInfo with an associated ImGui window 
				/// and entity ID.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_ExplorerFileUIView">The explorer file ui view.</param>
				ExplorerFileUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ImGuiUIView(a_Window), m_ExplorerFileUIView(a_ExplorerFileUIView)
				{}

				virtual ~ExplorerFileUIViewInfo() = default;

				/// <summary>
				/// Renders the file UI in the editor.
				/// </summary>
				void Render();

				/// <summary>
				/// Renders the file UI in the editor preview section.
				/// This function can be overridden by derived classes to render
				/// file-specific details.
				virtual void RenderPreview()
				{}

				bool GetShowPreview() const
				{
					return m_bShowPreview;
				}
			protected:
				/// <summary>
				/// Renders the file UI in the editor.
				/// This function can be overridden by derived classes to render
				/// file-specific details.
				virtual void RenderSpecific()
				{}

				bool m_bShowPreview = false;
				ExplorerFileUIView& m_ExplorerFileUIView;
			};
		}
	}
}

#pragma region REGISTER_EXPLORER_UI_CODE

#include <functional>
#include <unordered_map>

namespace gallus
{
	namespace resources
	{
		enum class AssetType;
	}
}

using ExplorerUIFactory = std::function<
	gallus::graphics::imgui::ExplorerFileUIViewInfo* (
		gallus::graphics::imgui::ImGuiWindow&,
		gallus::graphics::imgui::ExplorerFileUIView&
		)>;

inline auto& GetExplorerUIFactoryRegistry()
{
	static std::unordered_map<gallus::resources::AssetType, ExplorerUIFactory> registry;
	return registry;
}

struct ExplorerUIRegistrar
{
	ExplorerUIRegistrar(std::vector<gallus::resources::AssetType> types, ExplorerUIFactory factory)
	{
		for (auto type : types)
		{
			GetExplorerUIFactoryRegistry()[type] = factory; // copy per type
		}
	}
};

// Helper to produce unique names (more robust than raw _reg_##__COUNTER__)
#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define UNIQUE_NAME(base) CONCAT(base, __COUNTER__)

// Macro (unchanged semantics, but uses UNIQUE_NAME for the static var)
#define REGISTER_EXPLORER_UI(AssetTypesArray, UIViewType) \
    static ExplorerUIRegistrar UNIQUE_NAME(_reg_)(       \
        AssetTypesArray,                                 \
        [](gallus::graphics::imgui::ImGuiWindow& window, \
           gallus::graphics::imgui::ExplorerFileUIView& fileView) \
            -> gallus::graphics::imgui::ExplorerFileUIViewInfo* { \
            return new UIViewType(window, fileView);             \
        });

#pragma endregion REGISTER_EXPLORER_UI_CODE

#endif // _EDITOR
#endif // IMGUI_DISABLE