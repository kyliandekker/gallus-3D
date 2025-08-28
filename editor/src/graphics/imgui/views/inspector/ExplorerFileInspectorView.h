#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/views/inspector/InspectorView.h"

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
			class ExplorerFileInspectorView : public InspectorView
			{
			public:
				/// <summary>
				/// Constructs an inspector view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EntityID">The entity that will be shown in the view.</param>
				ExplorerFileInspectorView(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView);

				virtual ~ExplorerFileInspectorView();

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