#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

#include "graphics/imgui/views/DataTypes/StringTextInput.h"
#include "resources/AssetType.h"
#include "animation/AnimationTrack.h"
#include "editor/graphics/imgui/AnimationTrackUIView.h"

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
			class FileEditorSelectable;

			//---------------------------------------------------------------------
			// AnimationWindow
			//---------------------------------------------------------------------
			/// <summary>
			/// A window that displays and manages the logger messages of the project.
			/// </summary>
			class AnimationWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs a Animation window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				AnimationWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Cleans up and destroys the Animation window.
				/// </summary>
				~AnimationWindow();

				/// <summary>
				/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
				/// </summary>
				void Update() override;

				/// <summary>
				/// Renders the Animation window.
				/// </summary>
				void Render() override;

				/// <summary>
				/// Sets the data of the animation modal.
				/// </summary>
				/// <param name="a_File">The animation file.</param>
				void SetData(gallus::resources::FileResource& a_File);
			protected:
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