#pragma once

#include "imgui_system/windows/BaseWindow.h"

// graphics
#include "editor/graphics/imgui/views/StringTextInput.h"

// animation
#include "animation/Animation.h"

// resources
#include "resources/AssetType.h"

// editor
#include "editor/graphics/imgui/views/Toolbar.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
			class DX12Resource;
		}

		namespace imgui
		{
			class ImGuiSystem;
			class AnimationKeyFrameEditorSelectable;

			constexpr float ANIMATION_FRAME_PIXEL_WIDTH_DEFAULT = 25.0f;
			inline float ANIMATION_FRAME_PIXEL_WIDTH = 25.0f;
			constexpr float LEGEND_HEIGHT = 50.0f;
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
				/// <param name="a_System">The ImGui system for rendering the view.</param>
				AnimationWindow(ImGuiSystem& a_System);

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
				/// <param name="a_sID">The id for the resource.</param>
				void SetData(const std::string a_sID);
			protected:
				// Toolbar.
				void PopulateToolbar();
				void DrawToolbar();

				void RemoveKeyFrame(size_t a_iIndex);
				void AddKeyFrame(size_t a_iIndex);

				void SetAnimationDirty();

				void SetCurrentFrame(int16_t a_iIndex);

				std::shared_ptr<graphics::dx12::Texture> m_pPreviewTexture = nullptr;

				animation::Animation m_Animation;
				std::shared_ptr<AnimationKeyFrameEditorSelectable> m_pKeyFrameSelectable;

				int m_iSelectedFrame = 0;
				int m_iSelectedKeyFrame = -1;

				float m_fScroll = 1;

				Toolbar m_Toolbar;
			};
		}
	}
}