#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "editor/graphics/imgui/EditorSelectable.h"

// external
#include <string>
#include <vector>
#include <map>

namespace gallus
{
	namespace animation
	{
		class AnimationKeyFrame;
		class Animation;
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
			class AnimationKeyFrameEditorSelectable : public EditorSelectable
			{
			public:
				~AnimationKeyFrameEditorSelectable();

				/// <summary>
				/// Constructs an inspector view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_KeyFrame">The key frame that will be shown in the view.</param>
				/// <param name="a_Animation">The animation track related to the key frame.</param>
				AnimationKeyFrameEditorSelectable(ImGuiWindow& a_Window, animation::AnimationKeyFrame& a_KeyFrame, animation::Animation& a_Animation);

				std::string GetName() const override;

				std::string GetIcon() const override;

				void OnDelete() override;

				void RenderEditorFields() override;
			protected:
				animation::AnimationKeyFrame& m_KeyFrame;
				animation::Animation& m_Animation;

				std::map<std::string, bool> m_aExpanded; // A list of component booleans.
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE