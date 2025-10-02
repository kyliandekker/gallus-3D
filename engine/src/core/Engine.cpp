// header
#include "Engine.h"

// core
#include "ArgProcessor.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/file_abstractions.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// Engine
		//---------------------------------------------------------------------
		bool Engine::Initialize(HINSTANCE a_hInstance, const std::string& a_sName)
		{
			ENGINE = this;
#ifdef _TEST
			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif // _TEST
			// Initialize logger.
			// Logger is a global var unlike all the other systems. Not the prettiest but not too bad either.
			logger::LOGGER.Initialize(true);

			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Initializing engine.");

			m_ResourceAtlas.Initialize();

			// We initialize the window first and set the size and title after it has been created.
			m_Window.Initialize(true, a_hInstance);
			m_Window.SetTitle(a_sName);

			m_DX12.Initialize(true, m_Window.GetHWnd(), m_Window.GetRealSize(), &m_Window);

			m_ECS.Initialize();

			System::Initialize();

			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Initialized engine.");

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
		bool Engine::Destroy()
		{
			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Destroying engine.");

			m_ECS.Destroy();

			m_DX12.Destroy();

			m_Window.Destroy();

			m_ResourceAtlas.Destroy();

			// Destroy the logger last so we can see possible error messages from other systems.
			logger::LOGGER.Destroy();

			return System::Destroy();
		}

		//---------------------------------------------------------------------
		ResourceAtlas& Engine::GetResourceAtlas()
		{
			return m_ResourceAtlas;
		}

		//---------------------------------------------------------------------
		graphics::win32::Window& Engine::GetWindow()
		{
			return m_Window;
		}

		//---------------------------------------------------------------------
		graphics::dx12::DX12System2D& Engine::GetDX12()
		{
			return m_DX12;
		}

		//---------------------------------------------------------------------
		gameplay::EntityComponentSystem& Engine::GetECS()
		{
			return m_ECS;
		}

		//---------------------------------------------------------------------
		void Engine::SetSaveDirectory(const std::filesystem::path& a_sSaveDirectory)
		{
			m_sSaveDirectory = a_sSaveDirectory;
			file::CreateDirectory(a_sSaveDirectory);
		}

		//---------------------------------------------------------------------
		void Engine::SetDefaultArguments() const
		{
			core::ARGS.AddArgument<std::string>(ASSET_PATH_ARG, "./data/assets/");
			core::ARGS.AddArgument<LogSeverity>(ASSERT_LEVEL_ARG, LogSeverity::LOGSEVERITY_ERROR);
			core::ARGS.AddArgument<bool>(LOG_TO_FILE_ARG, false);
			core::ARGS.AddArgument<logger::LogType>(LOG_TO_FILE_ARG, logger::LogType::LOGTYPE_WITH_PARENT_FOLDER);
		}
	}
}