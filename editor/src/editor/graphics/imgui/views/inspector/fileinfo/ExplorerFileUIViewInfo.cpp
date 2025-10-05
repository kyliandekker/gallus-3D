#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ExplorerFileUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include "graphics/imgui/ImGuiWindow.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			void ExplorerFileUIViewInfo::Render()
			{
				RenderSpecific();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE