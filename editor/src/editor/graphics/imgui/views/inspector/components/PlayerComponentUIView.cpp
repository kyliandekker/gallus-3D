#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "PlayerComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/ImGuizmo.h>
#include <rapidjson/document.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// gameplay includes
#include "gameplay/systems/components/PlayerComponent.h"
#include "gameplay/systems/PlayerSystem.h"
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string PlayerComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void PlayerComponentUIView::RenderInner()
			{
				ImGui::Text("Test");
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE