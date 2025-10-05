#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerSceneUIViewInfo.h"

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

// resources
#include "resources/FileResource.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			ExplorerSceneUIViewInfo::ExplorerSceneUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ExplorerFileUIViewInfo(a_Window, a_ExplorerFileUIView)
			{
			}

			void ExplorerSceneUIViewInfo::RenderSpecific()
			{
				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_SCENE + std::string(" Open Scene"), BUTTON_ID, "OPEN_SCENE_FILE_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					core::Data data;
					m_ExplorerFileUIView.GetFileResource().GetFileData(data);

					gameplay::GAME.GetScene().SetData(data);
					gameplay::GAME.GetScene().SetPath(m_ExplorerFileUIView.GetFileResource().GetPath());
					gameplay::GAME.GetScene().LoadData();

					core::EDITOR_ENGINE->GetEditor().SetEditorMethod(editor::EditorMethod::EDITOR_METHOD_SCENE);

					core::EDITOR_ENGINE->GetEditor().GetScene().SetData(data);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetPath(m_ExplorerFileUIView.GetFileResource().GetPath());
					core::EDITOR_ENGINE->GetEditor().GetScene().LoadData();
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE