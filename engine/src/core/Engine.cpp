#include "Engine.h"

#include <glm/vec2.hpp>

#include "ArgProcessor.h"

// logger includes
#include "logger/Logger.h"

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

			const glm::ivec2 size = m_Window.GetRealSize();
			if (!m_DX12.Initialize(true, m_Window.GetHWnd(), size, &m_Window))
			{
				return false;
			}

			m_ECS.Initialize();

			m_InputSystem.Initialize(false);

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

			m_ECS.Destroy();

			m_DX12.Destroy();

			m_Window.Destroy();

			m_ResourceAtlas.Destroy();

			// Destroy the logger last so we can see possible error messages from other systems.
			logger::LOGGER.Destroy();

			return System::Destroy();
		}

		//---------------------------------------------------------------------
		resources::ResourceAtlas& Engine::GetResourceAtlas()
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
		input::InputSystem& Engine::GetInputSystem()
		{
			return m_InputSystem;
		}

		//---------------------------------------------------------------------
		gameplay::EntityComponentSystem& Engine::GetECS()
		{
			return m_ECS;
		}

		void Engine::SetDefaultArguments() const
		{
			gallus::core::ARGS.AddArgument<std::string>(ASSET_PATH_ARG, "");
			gallus::core::ARGS.AddArgument<gallus::LogSeverity>(ASSERT_LEVEL_ARG, gallus::LogSeverity::LOGSEVERITY_ERROR);
			gallus::core::ARGS.AddArgument<bool>(LOG_TO_FILE_ARG, false);
			gallus::core::ARGS.AddArgument<gallus::logger::LogType>(LOG_TO_FILE_ARG, gallus::logger::LogType::LOGTYPE_WITH_PARENT_FOLDER);
		}
	}
}