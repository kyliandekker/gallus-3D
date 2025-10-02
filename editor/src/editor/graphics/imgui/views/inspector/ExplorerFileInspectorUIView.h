#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

// base class
#include "editor/graphics/imgui/views/inspector/InspectorUIView.h"

// external
#include <string>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			class ExplorerFileUIView;
			class ExplorerFileUIViewInfo;

			/// <summary>
			/// Class that displays entities in the inspector.
			/// </summary>
			class ExplorerFileInspectorUIView : public InspectorUIView
			{
			public:
				/// <summary>
				/// Constructs an inspector view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EntityID">The entity that will be shown in the view.</param>
				ExplorerFileInspectorUIView(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView);

				virtual ~ExplorerFileInspectorUIView();

				void OnRename(const std::string& a_sName) override;

				void OnDelete() override;

				void OnShowInExplorer() override;

				std::string GetName() const override;

				std::string GetIcon() const override;

				void Render() override;

				void RenderPreview() override;
			protected:
				ExplorerFileUIView& m_ExplorerFileUIView;
				ExplorerFileUIViewInfo* m_pExplorerFileUIViewInfo = nullptr;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE