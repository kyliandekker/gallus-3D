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
			namespace editor
			{
				void ExplorerFileUIViewInfo::Render()
				{
					RenderSpecific();
				}
			}
		}
	}
}