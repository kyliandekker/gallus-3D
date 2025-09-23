#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

#include <string>

#include "editor/core/EditorEngine.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class DX12Transform;
		}
	}
	namespace gameplay
	{
		struct EntityID;
		class Component;
		class AbstractECSSystem;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			//---------------------------------------------------------------------
			// ComponentBaseUIView
			//---------------------------------------------------------------------
			/// <summary>
			/// Base class for rendering components in the ImGui editor UI. 
			/// Provides common functionality for rendering and interacting with 
			/// components and their associated systems within the entity-component system (ECS).
			/// </summary>
			class ComponentBaseUIView : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs the ComponentBaseUIView with an associated ImGui window 
				/// and entity ID.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EntityID">The EntityID associated with the component being rendered.</param>
				ComponentBaseUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID) : ImGuiUIView(a_Window), m_EntityID(a_EntityID)
				{}

				/// <summary>
				/// Renders the component UI in the editor.
				/// This function can be overridden by derived classes to render
				/// component-specific details.
				/// </summary>
				void Render()
				{};

				/// <summary>
				/// Renders the base component using the provided component and system.
				/// </summary>
				/// <param name="a_Component">The component to be rendered.</param>
				/// <param name="a_System">The system managing the component.</param>
				void RenderBaseComponent(gameplay::Component& a_Component, gameplay::AbstractECSSystem& a_System);

				bool ShowPreview() const
				{
					return m_bShowPreview;
				}

				int GetPreviewPriority() const
				{
					return m_iPreviewPriority;
				}

				virtual void RenderPreview()
				{}

				virtual void RenderComponentGizmos(const ImVec2 & a_vScenePos, const ImVec2 & a_vSize, const ImVec2& a_vPanOffset, float a_fZoom)
				{}
			protected:
				gameplay::EntityID& m_EntityID; // Reference to the entity's ID. Used for associating the component with an entity.

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

				void DrawGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom);

				void DrawTransformGizmo(graphics::dx12::DX12Transform& a_Transform, const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom);

				bool m_bFoldedOut = true; /// Indicates whether the component UI is folded out or not.
				bool m_bShowPreview = true; /// Indicates whether the component renders a preview or not.
				int m_iPreviewPriority = 0;
			};

			/// <summary>
			/// Template class for rendering specific component UIs in the editor.
			/// Inherits from ComponentBaseUIView and allows the rendering of specific
			/// component types with their corresponding system.
			/// </summary>
			/// <typeparam name="C">The component type.</typeparam>
			/// <typeparam name="S">The system type managing the component.</typeparam>
			template <class C, class S>
			class ComponentUIView : public ComponentBaseUIView
			{
			public:
				/// <summary>
				/// Constructs the ComponentUIView with a specific component and system.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EntityID">The EntityID associated with the component being rendered.</param>
				/// <param name="a_Component">The component to render.</param>
				/// <param name="a_System">The system managing the component.</param>
				ComponentUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, C& a_Component, S& a_System) : ComponentBaseUIView(a_Window, a_EntityID), m_Component(a_Component), m_System(a_System)
				{}

				/// <summary>
				/// Renders the component's UI using the base render method.
				/// </summary>
				void Render() override
				{
					std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

					RenderBaseComponent(m_Component, m_System);
				}

				/// <summary>
				/// Render the inner part of the UI, specific to each derived class.
				/// </summary>
				void RenderInner() override = 0;
			protected:
				C& m_Component; /// Reference to the specific component being rendered.
				S& m_System; /// Reference to the system managing the component.
			};
		}
	}
}

#pragma region REGISTER_COMPONENT_UI_CODE

#include <functional>
#include <typeindex>
#include <unordered_map>

using ComponentUIFactory = std::function<
	gallus::graphics::imgui::ComponentBaseUIView* (
		gallus::graphics::imgui::ImGuiWindow&,
		gallus::gameplay::EntityID&
		)>;

inline auto& GetComponentUIFactoryRegistry()
{
	static std::unordered_map<std::type_index, ComponentUIFactory> registry;
	return registry;
}

struct ComponentUIRegistrar
{
	ComponentUIRegistrar(std::type_index type, ComponentUIFactory factory)
	{
		GetComponentUIFactoryRegistry()[type] = std::move(factory);
	}
};

#define REGISTER_COMPONENT_UI(ComponentType, UIViewType, SystemType) \
    static ComponentUIRegistrar _reg_##__COUNTER__( \
        typeid(ComponentType), \
        [](gallus::graphics::imgui::ImGuiWindow& window, gallus::gameplay::EntityID& entityId) \
            -> gallus::graphics::imgui::ComponentBaseUIView* { \
            auto& sys = gallus::core::EDITOR_ENGINE->GetECS().GetSystem<SystemType>(); \
            if (sys.HasComponent(entityId)) { \
                auto& comp = sys.GetComponent(entityId); \
                return new UIViewType(window, entityId, comp, sys); \
            } \
            return nullptr; \
        });

#pragma endregion REGISTER_COMPONENT_UI_CODE

#endif // _EDITOR
#endif // IMGUI_DISABLE