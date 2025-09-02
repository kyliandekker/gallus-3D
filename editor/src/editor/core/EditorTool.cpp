#include "EditorTool.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// EditorTool
		//---------------------------------------------------------------------
		EditorTool::EditorTool() : Tool::Tool()
		{}

		//---------------------------------------------------------------------
		bool EditorTool::Initialize(HINSTANCE a_hInstance, const std::string& a_sName)
		{
			Tool::Initialize(a_hInstance, a_sName);

			EDITOR_TOOL = this;

			m_Editor.Initialize(true);

			return true;
		}

		//---------------------------------------------------------------------
		bool EditorTool::Destroy()
		{
			m_Editor.Destroy();

			return Tool::Destroy();
		}
	}
}