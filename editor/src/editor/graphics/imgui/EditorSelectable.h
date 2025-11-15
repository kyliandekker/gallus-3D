#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include <imgui/imgui.h>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class EditorSelectable
			{
			public:
				virtual void RenderGizmos(
					const ImVec2& a_vScenePos,
					const ImVec2& a_vSize,
					const ImVec2& a_vPanOffset,
					float a_fZoom)
				{ }
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE