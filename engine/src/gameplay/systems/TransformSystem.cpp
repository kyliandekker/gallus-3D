#include "gameplay/systems/TransformSystem.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// TransformSystem
		//---------------------------------------------------------------------
		bool TransformSystem::Initialize()
		{
			m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME_APPLY);

			LOG_ICON(font::ICON_AXIS, LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Transform system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string TransformSystem::GetPropertyName() const
		{
			return "transform";
		}

		//---------------------------------------------------------------------
		std::string TransformSystem::GetSystemName() const
		{
			std::string name = "Transform";
#ifdef _EDITOR
			name = std::string(font::ICON_AXIS) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}