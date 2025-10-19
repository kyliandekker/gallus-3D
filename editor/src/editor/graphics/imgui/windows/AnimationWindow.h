#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			//---------------------------------------------------------------------
			// AnimationWindow
			//---------------------------------------------------------------------
			/// <summary>
			/// A window that displays and allows editing of animations.
			/// </summary>
			class AnimationWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs a animation window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				AnimationWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Cleans up and destroys the animation window.
				/// </summary>
				~AnimationWindow();

				/// <summary>
				/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
				/// </summary>
				void Update() override;

				/// <summary>
				/// Renders the animation window.
				/// </summary>
				void Render() override;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE