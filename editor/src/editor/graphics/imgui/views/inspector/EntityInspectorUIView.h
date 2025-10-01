#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "editor/graphics/imgui/views/inspector/InspectorUIView.h"

#include <string>
#include <vector>

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
			class EntityInspectorUIView : public InspectorUIView
			{
			public:
				~EntityInspectorUIView();

				/// <summary>
				/// Constructs an inspector view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_HierarchyEntityUIView">The entity that will be shown in the view.</param>
				EntityInspectorUIView(ImGuiWindow& a_Window, HierarchyEntityUIView& a_HierarchyEntityUIView);

				void OnRename(const std::string& a_sName) override;

				void OnDelete() override;

				std::string GetName() const override;

				std::string GetIcon() const override;

				void Render() override;

				void RenderPreview() override;

				std::vector<ComponentBaseUIView*>& GetComponents()
				{
					return m_aComponents;
				}
			protected:
				HierarchyEntityUIView& m_HierarchyEntityUIView;
				gameplay::Entity* m_pEntity = nullptr;

				std::vector<ComponentBaseUIView*> m_aComponents; /// A list of component UI views associated with this entity.
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE