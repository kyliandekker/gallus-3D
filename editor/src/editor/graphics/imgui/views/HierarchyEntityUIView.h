#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

#include <string>

// editor includes
#include "editor/graphics/imgui/EditorSelectable.h"

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

			class HierarchyEntityUIView : public ImGuiUIView, public EditorSelectable
			{
			public:
				HierarchyEntityUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID);

				/// <summary>
				/// Renders the entity UI with selection and click interaction.
				/// </summary>
				/// <param name="a_bClicked">Reference to a boolean indicating if the entity was clicked.</param>
				/// <param name="a_bDoubleClicked">Reference to a boolean indicating if the entity was double clicked.</param>
				/// <param name="a_bSelected">Boolean indicating if the entity is currently selected.</param>
				void RenderEntity(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected);

				void Render() override
				{}

				gameplay::EntityID& GetEntityID()
				{
					return m_EntityID;
				}

				const gameplay::EntityID& GetEntityID() const
				{
					return m_EntityID;
				}

				const std::string& GetIcon()
				{
					return m_sIcon;
				}
			private:
				gameplay::EntityID& m_EntityID;
				std::string m_sIcon;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE