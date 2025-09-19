#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>

#include "core/Tool.h"

namespace gallus
{
	namespace gameplay
	{
		class EntityID;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ComponentBaseUIView;
			class ImGuiWindow;
		}
	}
}

// Factory signature
using ComponentUIFactory =
std::function<gallus::graphics::imgui::ComponentBaseUIView* (
	gallus::graphics::imgui::ImGuiWindow&,
	gallus::gameplay::EntityID&)>;

// Registry
inline std::unordered_map<std::type_index, ComponentUIFactory> g_UIFactories;

// Macro that accepts the system TYPE
#define REGISTER_COMPONENT_UI(ComponentType, UIViewType, SystemType) \
    g_UIFactories[typeid(ComponentType)] = \
        [](gallus::graphics::imgui::ImGuiWindow& window, gallus::gameplay::EntityID& entityId) \
            -> gallus::graphics::imgui::ComponentBaseUIView* { \
            auto& ecs = gallus::core::TOOL->GetECS(); \
            auto& sys = ecs.GetSystem<SystemType>(); \
            if (sys.HasComponent(entityId)) { \
                auto& comp = sys.GetComponent(entityId); \
                return new UIViewType(window, entityId, comp, sys); \
            } \
            return nullptr; \
        }

namespace gallus
{
	namespace editor
	{
		void RegisterComponents();
	}
}