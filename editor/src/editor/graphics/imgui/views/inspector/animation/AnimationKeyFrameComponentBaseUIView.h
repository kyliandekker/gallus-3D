#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

#include <string>

#include "editor/core/EditorEngine.h"

#include "animation/AnimationKeyFrame.h"

namespace gallus
{
	namespace animation
	{
		class AnimationKeyFrameComponentBase;
		class AnimationKeyFrame;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			//---------------------------------------------------------------------
			// AnimationKeyFrameComponentBaseUIView
			//---------------------------------------------------------------------
			/// <summary>
			/// Base class for rendering components in the ImGui editor UI. 
			/// Provides common functionality for rendering and interacting with key frames and their components.
			/// </summary>
			class AnimationKeyFrameComponentBaseUIView : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs the AnimationKeyFrameComponentBaseUIView with an associated ImGui window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				AnimationKeyFrameComponentBaseUIView(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
				{}

				/// <summary>
				/// Renders the component UI in the editor.
				/// This function can be overridden by derived classes to render
				/// component-specific details.
				/// </summary>
				void Render()
				{};

				/// <summary>
				/// Renders the base component using the provided component.
				/// </summary>
				/// <param name="a_Component">The component to be rendered.</param>
				void RenderBaseComponent(animation::AnimationKeyFrameComponentBase& a_Component);
			protected:
				/// <summary>
				/// Render the inner part of the UI, specific to each derived class.
				/// </summary>
				virtual void RenderInner() = 0;

				/// <summary>
				/// Retrieves the name of the component UI.
				/// </summary>
				/// <returns>The name of the UI component.</returns>
				virtual std::string GetName() const
				{
					return "";
				}

				bool m_bFoldedOut = true; /// Indicates whether the component UI is folded out or not.
			};

			/// <summary>
			/// Template class for rendering specific component UIs in the editor.
			/// Inherits from ComponentBaseUIView and allows the rendering of specific
			/// component types.
			/// </summary>
			/// <typeparam name="C">The component type.</typeparam>
			template <class C>
			class AnimationKeyFrameComponentUIView : public AnimationKeyFrameComponentBaseUIView
			{
			public:
				/// <summary>
				/// Constructs the ComponentUIView with a specific component and system.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_Component">The component to render.</param>
				/// <param name="a_System">The system managing the component.</param>
				AnimationKeyFrameComponentUIView(ImGuiWindow& a_Window, C& a_Component) : AnimationKeyFrameComponentBaseUIView(a_Window), m_Component(a_Component)
				{}

				/// <summary>
				/// Renders the component's UI using the base render method.
				/// </summary>
				void Render() override
				{
					RenderBaseComponent(m_Component);
				}

				/// <summary>
				/// Render the inner part of the UI, specific to each derived class.
				/// </summary>
				void RenderInner() override = 0;
			protected:
				C& m_Component; /// Reference to the specific component being rendered.
			};
		}
	}
}

#pragma region REGISTER_KEY_FRAME_COMPONENT_UI_CODE

#include <functional>
#include <typeindex>
#include <unordered_map>

using KeyFrameComponentUIFactory = std::function<
	gallus::graphics::imgui::AnimationKeyFrameComponentBaseUIView* (
		gallus::graphics::imgui::ImGuiWindow&,
		gallus::animation::AnimationKeyFrame&
	)>;

inline auto& GetKeyFrameComponentUIFactoryRegistry()
{
	static std::unordered_map<std::type_index, KeyFrameComponentUIFactory> registry;
	return registry;
}

struct KeyFrameComponentUIRegistrar
{
	KeyFrameComponentUIRegistrar(
		std::type_index type,
		KeyFrameComponentUIFactory factory
	)
	{
		GetKeyFrameComponentUIFactoryRegistry()[type] = std::move(factory);
	}
};

#define REGISTER_KEY_FRAME_COMPONENT_UI(ComponentType, UIViewType) \
	static KeyFrameComponentUIRegistrar _reg_##__COUNTER__( \
		typeid(ComponentType), \
		[](gallus::graphics::imgui::ImGuiWindow& a_Window, gallus::animation::AnimationKeyFrame& a_KeyFrame) \
			-> gallus::graphics::imgui::AnimationKeyFrameComponentBaseUIView* { \
			if (a_KeyFrame.HasComponent<ComponentType>()) { \
				auto& comp = *a_KeyFrame.GetComponent<ComponentType>(); \
				return new UIViewType(a_Window, comp); \
			} \
			return nullptr; \
		});

#pragma endregion REGISTER_KEY_FRAME_COMPONENT_UI_CODE

#endif // _EDITOR
#endif // IMGUI_DISABLE
