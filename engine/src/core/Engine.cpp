#include "Engine.h"

// core
#include "core/ArgProcessor.h"

// gameplay
#include "gameplay/ECSBaseSystem.h"

// systems
#include "graphics/dx12/DX12System.h"
#include "graphics/win32/Window.h"
#include "input/InputSystem.h"
#include "resources/ResourceAtlas.h"
#include "gameplay/EntityComponentSystem.h"

// logger
#include "logger/Logger.h"

namespace gallus::core
{
	//---------------------------------------------------------------------
	// Engine
	//---------------------------------------------------------------------
	bool Engine::Initialize(HINSTANCE a_hInstance, const std::string& a_sName)
	{
#ifdef _TEST
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif // _TEST
	// Initialize logger first.
	if (!m_pLogger->Initialize(true))
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Failed initializing engine: Logger could not be initialized.");
		return false;
	}
	
	LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Initializing engine.");
	
	if (!m_pResourceAtlas->Initialize())
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Failed initializing engine: Resource atlas could not be initialized.");
		return false;
	}
	
	if (!m_pWindow->Initialize(true, a_hInstance))
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Failed initializing engine: Window could not be initialized.");
		return false;
	}
	m_pWindow->SetTitle(a_sName);
	
	const IVector2 size = m_pWindow->GetRealSize();
	if (!m_pDX12->Initialize(true, m_pWindow->GetHWnd(), size, m_pWindow.get(), 4))
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Failed initializing engine: DX12 System could not be initialized.");
		return false;
	}
	
	if (!m_pECS->Initialize())
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Failed initializing engine: ECS could not be initialized.");
		return false;
	}
	
	if (!m_pInputSystem->Initialize(false))
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Failed initializing engine: Input System could not be initialized.");
		return false;
	}
	
	System::Initialize();
	
	LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ENGINE, "Successfully initialized engine.");
	
#ifdef _TEST
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::string microseconds = "Initialization took " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) + " microseconds.";
	std::string nanoseconds = "Initialization took " + std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) + " nanoseconds.";
	std::string milliseconds = "Initialization took " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()) + " milliseconds.";
	std::string seconds = "Initialization took " + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()) + " seconds.";

	TEST(microseconds.c_str());
	TEST(nanoseconds.c_str());
	TEST(milliseconds.c_str());
	TEST(seconds.c_str());
#endif // _TEST

return true;
	}

	//---------------------------------------------------------------------
	Engine::Engine()
	{
		m_pLogger = std::make_unique<logger::Logger>();
		m_pResourceAtlas = std::make_unique<resources::ResourceAtlas>();
		m_pWindow = std::make_unique<graphics::win32::Window>();
		m_pDX12 = std::make_unique<graphics::dx12::DX12System>();
		m_pECS = std::make_unique<gameplay::EntityComponentSystem>();
		m_pInputSystem = std::make_unique<input::InputSystem>();
	}

	//---------------------------------------------------------------------
	Engine::~Engine() = default;

	//---------------------------------------------------------------------
	bool Engine::Destroy()
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Destroying engine.");

		m_pECS->Destroy();

		m_pDX12->Destroy();

		m_pWindow->Destroy();

		m_pResourceAtlas->Destroy();

		LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_ENGINE, "Successfully destroyed engine.");

		// Destroy the logger last so we can see possible error messages from other systems.
		m_pLogger->Destroy();

		return System::Destroy();
	}
	
	//---------------------------------------------------------------------
	logger::Logger* Engine::GetLogger()
	{
		return m_pLogger.get();
	}

	//---------------------------------------------------------------------
	resources::ResourceAtlas* Engine::GetResourceAtlas()
	{
		return m_pResourceAtlas.get();
	}

	//---------------------------------------------------------------------
	graphics::win32::Window* Engine::GetWindow()
	{
		return m_pWindow.get();
	}

	//---------------------------------------------------------------------
	graphics::dx12::DX12System* Engine::GetDX12()
	{
		return m_pDX12.get();
	}

	//---------------------------------------------------------------------
	input::InputSystem* Engine::GetInputSystem()
	{
		return m_pInputSystem.get();
	}

	//---------------------------------------------------------------------
	gameplay::EntityComponentSystem* Engine::GetECS()
	{
		return m_pECS.get();
	}

	//---------------------------------------------------------------------
	void Engine::SetDefaultArguments() const
	{
		core::ARGS.AddArgument<std::string>(ASSET_PATH_ARG, "");
		core::ARGS.AddArgument<LogSeverity>(ASSERT_LEVEL_ARG, LogSeverity::LOGSEVERITY_ASSERT);
		core::ARGS.AddArgument<bool>(LOG_TO_FILE_ARG, false);
		core::ARGS.AddArgument<logger::LogType>(LOG_TO_FILE_ARG, logger::LogType::LOGTYPE_WITH_PARENT_FOLDER);
	}
}