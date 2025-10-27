#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/modals/BaseModal.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "graphics/imgui/views/DataTypes/StringTextInput.h"
#include "resources/AssetType.h"
#include "animation/AnimationTrack.h"
#include "editor/graphics/imgui/views/inspector/animation/AnimationTrackUIView.h"

namespace gallus
{
	namespace resources
	{
		class FileResource;
	}
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
			class DX12Resource;
		}

		namespace imgui
		{
			class ImGuiWindow;

			class ExplorerFileUIView;

			class AnimationEditorModal : public BaseModal
			{
			public:
				/// <summary>
				/// Constructs a window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				AnimationEditorModal(ImGuiWindow& a_Window);

				/// <summary>
				/// Renders the element.
				/// </summary>
				void Render() override;

				void Show() override;

				/// <summary>
				/// Sets the data of the animation modal.
				/// </summary>
				/// <param name="a_File">The animation file.</param>
				void SetData(gallus::resources::FileResource& a_File);
			private:
				std::shared_ptr<graphics::dx12::Texture> m_pPreviewTexture = nullptr;
				gallus::resources::FileResource* m_pFile = nullptr;

				animation::AnimationTrack m_AnimationTrack;
				AnimationTrackUIView m_AnimationTrackUIView;

				int m_iCurrentFrame = 0;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE