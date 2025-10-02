// header
#include "MovementSystem.h"

// graphics
#include "graphics/imgui/font_icon.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/systems/CollisionSystem.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// MovementSystem
		//---------------------------------------------------------------------
		bool MovementSystem::Initialize()
		{
			m_UpdateTime = UpdateTime::UPDATE_TIME_END_FRAME;

			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Movement system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string MovementSystem::GetPropertyName() const
		{
			return "movement";
		}

		//---------------------------------------------------------------------
		std::string MovementSystem::GetSystemName() const
		{
			std::string name = "Movement";
		#ifdef _EDITOR
			name = std::string(font::ICON_TRANSLATE) + " " + name;
		#endif // _EDITOR
			return name;
		}

		//---------------------------------------------------------------------
		void MovementSystem::UpdateComponentsRealtime(float a_fDeltatime)
		{
			for (auto& component : m_mComponents)
			{
				component.second.UpdateRealtime(a_fDeltatime, core::ENGINE->GetECS().GetSystem<CollisionSystem>().GetComponents());
			}
		}
	}
}