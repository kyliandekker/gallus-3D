#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ColliderComponentUIView.h"

#include <imgui/imgui_helpers.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string ColliderComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void ColliderComponentUIView::RenderInner()
			{
				ImGui::Text("Test");
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE