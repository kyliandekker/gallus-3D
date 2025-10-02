// header
#include "EditorEngine.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// EditorEngine
		//---------------------------------------------------------------------
		EditorEngine::EditorEngine() : Engine::Engine()
		{}

		//---------------------------------------------------------------------
		bool EditorEngine::Initialize(HINSTANCE a_hInstance, const std::string& a_sName)
		{
			Engine::Initialize(a_hInstance, a_sName);

			EDITOR_ENGINE = this;

			m_Editor.Initialize(true);

			return true;
		}

		//---------------------------------------------------------------------
		bool EditorEngine::Destroy()
		{
			m_Editor.Destroy();

			return Engine::Destroy();
		}
	}
}