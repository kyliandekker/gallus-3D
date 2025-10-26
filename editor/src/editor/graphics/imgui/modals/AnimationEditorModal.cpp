#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "AnimationEditorModal.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// core includes
#include "editor/core/EditorEngine.h"

// utils includes
#include "utils/string_extensions.h"

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"

// graphics includes
#include "resources/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			AnimationEditorModal::AnimationEditorModal(ImGuiWindow& a_Window) : BaseModal(a_Window, std::string(font::ICON_ANIMATION) + " Animation Editor", "AnimationEditor")
			{
			}

			void AnimationEditorModal::Render()
			{
			}


			void AnimationEditorModal::Show()
			{
				BaseModal::Show();
			}

			void AnimationEditorModal::SetData(gallus::resources::FileResource& a_File)
			{
				m_pFile = &a_File;
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE