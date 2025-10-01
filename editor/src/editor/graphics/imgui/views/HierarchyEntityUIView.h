#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "editor/graphics/imgui/views/HierarchyUIView.h"

#include <string>

namespace gallus
{
	namespace gameplay
	{
		class EntityID;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			class HierarchyEntityUIView : public HierarchyUIView
			{
			public:
				HierarchyEntityUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID);

				/// <summary>
				/// Renders the entity UI with selection and click interaction.
				/// </summary>
				/// <param name="a_bClicked">Reference to a boolean indicating if the entity was clicked.</param>
				/// <param name="a_bDoubleClicked">Reference to a boolean indicating if the entity was double clicked.</param>
				/// <param name="a_bSelected">Boolean indicating if the entity is currently selected.</param>
				void RenderInHierarchy(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected) override;

				gameplay::EntityID& GetEntityID()
				{
					return m_EntityID;
				}

				const gameplay::EntityID& GetEntityID() const
				{
					return m_EntityID;
				}
			private:
				gameplay::EntityID& m_EntityID;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE