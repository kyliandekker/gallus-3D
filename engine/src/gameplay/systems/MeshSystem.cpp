#include "MeshSystem.h"

// graphics
#include "graphics/imgui/font_icon.h"

// logger
#include "logger/Logger.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// MeshSystem
		//---------------------------------------------------------------------
		bool MeshSystem::Initialize()
		{
			LOG_ICON(font::ICON_MODEL, LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Mesh system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string MeshSystem::GetPropertyName() const
		{
			return "mesh";
		}

		//---------------------------------------------------------------------
		std::string MeshSystem::GetSystemName() const
		{
			std::string name = "Mesh";
#ifdef _EDITOR
			name = std::string(font::ICON_MODEL) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}