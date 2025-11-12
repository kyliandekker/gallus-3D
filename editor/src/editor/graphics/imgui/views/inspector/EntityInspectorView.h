#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "editor/graphics/imgui/views/inspector/InspectorView.h"

#include <string>
#include <map>

namespace gallus
{
	namespace gameplay
	{
		class Entity;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			class HierarchyEntityUIView;
			class ComponentBaseUIView;

			/// <summary>
			/// Class that displays entities in the inspector.
			/// </summary>
			class EntityInspectorView : public InspectorView
			{
			public:
				~EntityInspectorView();

				/// <summary>
				/// Constructs an inspector view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_HierarchyEntityUIView">The entity that will be shown in the view.</param>
				EntityInspectorView(ImGuiWindow& a_Window, HierarchyEntityUIView& a_HierarchyEntityUIView);

				void OnRename(const std::string& a_sName) override;

				void OnDelete() override;

				std::string GetName() const override;

				std::string GetIcon() const override;

				void Render() override;

				void RenderPreview() override;
			protected:
				HierarchyEntityUIView& m_HierarchyEntityUIView;
				gameplay::Entity* m_pEntity = nullptr;

				std::map<std::string, bool> m_aExpanded; // A list of component booleans.
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE