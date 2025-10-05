#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ProjectileComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_toggle.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// gameplay includes
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string ProjectileComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void ProjectileComponentUIView::RenderInner()
			{
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE