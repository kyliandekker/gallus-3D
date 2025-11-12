#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "editor/graphics/imgui/views/inspector/InspectorView.h"
#include "editor/graphics/imgui/EditorSelectable.h"

#include <string>
#include <vector>
#include <map>

namespace gallus
{
	namespace animation
	{
		class AnimationKeyFrame;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;
			class AnimationKeyFrameComponentBaseUIView;

			/// <summary>
			/// Class that displays entities in the inspector.
			/// </summary>
			class AnimationKeyFrameUIView : public EditorSelectable
			{ };

			/// <summary>
			/// Class that displays entities in the inspector.
			/// </summary>
			class AnimationKeyFrameInspectorUIView : public InspectorView
			{
			public:
				~AnimationKeyFrameInspectorUIView();

				/// <summary>
				/// Constructs an inspector view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_KeyFrame">The key frame that will be shown in the view.</param>
				AnimationKeyFrameInspectorUIView(ImGuiWindow& a_Window, animation::AnimationKeyFrame& a_KeyFrame);

				std::string GetName() const override;

				std::string GetIcon() const override;

				void Render() override;
			protected:
				animation::AnimationKeyFrame& m_KeyFrame;

				std::map<std::string, bool> m_aExpanded; // A list of component booleans.
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE