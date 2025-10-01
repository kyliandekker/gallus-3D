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

			class HierarchyUIView : public ImGuiUIView, public EditorSelectable
			{
			public:
				HierarchyUIView(ImGuiWindow& a_Window) : ImGuiUIView(a_Window), EditorSelectable()
				{}

				/// <summary>
				/// Renders the entity UI with selection and click interaction.
				/// </summary>
				/// <param name="a_bClicked">Reference to a boolean indicating if the entity was clicked.</param>
				/// <param name="a_bDoubleClicked">Reference to a boolean indicating if the entity was double clicked.</param>
				/// <param name="a_bSelected">Boolean indicating if the entity is currently selected.</param>
				virtual void RenderInHierarchy(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected);

				void Render() override
				{}

				const std::string& GetIcon()
				{
					return m_sIcon;
				}

				void SetIcon(const std::string& a_sIcon)
				{
					m_sIcon = a_sIcon;
				}

				void SetName(const std::string& a_sName)
				{
					m_sName = a_sName;
				}

				const std::string& GetName()
				{
					return m_sName;
				}
			protected:
				std::string m_sIcon;
				std::string m_sName;
			};

			constexpr float HIERARCHY_CHILD_SIZE = 32;
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE