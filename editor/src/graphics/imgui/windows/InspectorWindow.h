#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "graphics/imgui/windows/BaseWindow.h"
#include "graphics/imgui/views/DataTypes/StringTextInput.h"
#include "graphics/imgui/views/ExplorerFileUIView.h"
#include "graphics/imgui/views/DataTypes/StringTextInput.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			class InspectorView;

			/// <summary>
			/// A window that displays the current editor selectable.
			/// </summary>
			class InspectorWindow : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs an inspector window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				InspectorWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Destroys and disables the inspector window.
				/// </summary>
				/// <returns>True if destruction is successful, otherwise false.</returns>
				bool Destroy() override;

				/// <summary>
				/// Renders the inspector window.
				/// </summary>
				void Render() override;
			private:
				StringTextInput m_NameInput; /// Inspector input for editing the entity's name.
			};
		}
	}
}

#endif // IMGUI_DISABLE
#endif // _EDITOR