#include "EditorEngine.h"

// systems
#include "graphics/dx12/DX12System.h"
#include "graphics/win32/Window.h"
#include "input/InputSystem.h"
#include "resources/ResourceAtlas.h"
#include "gameplay/EntityComponentSystem.h"
#include "utils/file_abstractions.h"

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
		EditorEngine::~EditorEngine() = default;

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