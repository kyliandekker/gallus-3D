#include "ComponentFactory.h"

// gameplay includes
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/PlayerSystem.h"

// editor includes
#include "editor/graphics/imgui/views/inspector/components/TransformComponentUIView.h"
#include "editor/graphics/imgui/views/inspector/components/SpriteComponentUIView.h"
#include "editor/graphics/imgui/views/inspector/components/PlayerComponentUIView.h"

namespace gallus
{
	namespace editor
	{
		void RegisterComponents()
		{
			REGISTER_COMPONENT_UI(gallus::gameplay::TransformComponent,
				gallus::graphics::imgui::TransformComponentUIView,
				gallus::gameplay::TransformSystem);

			REGISTER_COMPONENT_UI(gallus::gameplay::SpriteComponent,
				gallus::graphics::imgui::SpriteComponentUIView,
				gallus::gameplay::SpriteSystem);

			REGISTER_COMPONENT_UI(gallus::gameplay::PlayerComponent,
				gallus::graphics::imgui::PlayerComponentUIView,
				gallus::gameplay::PlayerSystem);
		}
	}
}