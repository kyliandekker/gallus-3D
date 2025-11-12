#include "gameplay/systems/RigidbodySystem.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// RigidbodySystem
		//---------------------------------------------------------------------
		bool RigidbodySystem::Initialize()
		{
			m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME);

			LOG_ICON(font::ICON_HEART, LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Rigidbody system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string RigidbodySystem::GetPropertyName() const
		{
			return "rigidbody";
		}

		//---------------------------------------------------------------------
		std::string RigidbodySystem::GetSystemName() const
		{
			std::string name = "Rigidbody";
		#ifdef _EDITOR
			name = std::string(font::ICON_HEART) + " " + name;
		#endif // _EDITOR
			return name;
		}
	}
}