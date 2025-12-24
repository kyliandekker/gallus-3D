#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

// external
#include <string>
#include <map>

// editor
#include "editor/graphics/imgui/EditorSelectable.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			class DirectionalLightEditorSelectable : public EditorSelectable
			{
			public:
				DirectionalLightEditorSelectable(ImGuiWindow& a_Window);

				void RenderEntity(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected);

				void Render() override
				{
				}

				std::string GetName() const override;

				std::string GetIcon() const override;

				void RenderEditorFields() override;
			private:
				std::string m_sIcon;
				bool m_bExpanded = false;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE