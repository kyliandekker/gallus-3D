#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

// graphics
#include "graphics/imgui/views/DataTypes/StringTextInput.h"

// animation
#include "animation/Animation.h"

// resources
#include "resources/AssetType.h"

// editor
#include "editor/graphics/imgui/selectables/AnimationKeyFrameEditorSelectable.h"
#include "editor/graphics/imgui/views/Toolbar.h"

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

			constexpr float ANIMATION_FRAME_PIXEL_WIDTH_DEFAULT = 25.0f;
			inline float ANIMATION_FRAME_PIXEL_WIDTH = 25.0f;
			constexpr float LEGEND_PADDING = 25.0f;
			constexpr float LEGEND_HEIGHT = 70.0f;
			constexpr float TRACK_SIZE = 50;

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
				/// Initializes all values and behaviours associated with the hierarchy window.
				/// </summary>
				/// <returns>True if initialization is successful, otherwise false.</returns>
				bool Initialize() override;

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
				// Toolbar.
				void PopulateToolbar();
				void DrawToolbar();

				void SetCurrentFrame(int a_iIndex);

				std::shared_ptr<graphics::dx12::Texture> m_pPreviewTexture = nullptr;
				gallus::resources::FileResource* m_pFile = nullptr;

				animation::Animation m_Animation;
				std::vector<AnimationKeyFrameEditorSelectable> m_KeyFrameSelectables;

				int m_iCurrentFrame = 0;
				int m_iSelectedKeyFrame = -1;

				Toolbar m_Toolbar;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE