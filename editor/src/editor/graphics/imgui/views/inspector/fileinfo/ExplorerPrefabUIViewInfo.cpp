#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerPrefabUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// core includes
#include "core/Data.h"

// graphics includes
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/imgui/font_icon.h"

// editor includes
#include "resources/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/core/EditorEngine.h"

// game includes
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			ExplorerPrefabUIViewInfo::ExplorerPrefabUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ExplorerFileUIViewInfo(a_Window, a_ExplorerFileUIView)
			{ }

			void ExplorerPrefabUIViewInfo::RenderSpecific()
			{
				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_MODEL + std::string(" Open Prefab"), BUTTON_ID, "OPEN_PREFAB_FILE_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					core::Data data;
					m_ExplorerFileUIView.GetFileResource().GetFileData(data);

					gameplay::GAME.GetScene().SetData(core::Data());
					gameplay::GAME.GetScene().SetPath("");

					core::EDITOR_ENGINE->GetEditor().SetEditorMethod(editor::EditorMethod::EDITOR_METHOD_PREFAB);

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