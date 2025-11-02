#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerAnimationUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// core includes
#include "core/Data.h"

// graphics includes
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/imgui/font_icon.h"

// editor includes
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/core/EditorEngine.h"

// game includes
#include "gameplay/Game.h"
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/TransformSystem.h"

// resources
#include "resources/FileResource.h"
#include <editor/graphics/imgui/EditorWindowsConfig.h>
#include <imgui/imgui_internal.h>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			ExplorerAnimationUIViewInfo::ExplorerAnimationUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ExplorerFileUIViewInfo(a_Window, a_ExplorerFileUIView)
			{
			}

			void ExplorerAnimationUIViewInfo::RenderSpecific()
			{
				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_MODEL + std::string(" Open Animation"), BUTTON_ID, "OPEN_ANIMATION_FILE_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					AnimationWindow& animationWindow = m_Window.GetWindowsConfig<EditorWindowsConfig>().GetAnimationWindow();

					animationWindow.SetData(
						m_ExplorerFileUIView.GetFileResource()
					);

					animationWindow.Focus();
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE