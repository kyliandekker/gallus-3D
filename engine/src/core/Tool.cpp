#include "Tool.h"

#include <glm/vec2.hpp>

// logger includes
#include "logger/Logger.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// Tool
		//---------------------------------------------------------------------
		bool Tool::Initialize(HINSTANCE a_hInstance, const std::string& a_sName)
		{
			TOOL = this;

#ifdef _TEST
			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif // _TEST
			// Initialize logger.
			// Logger is a global var unlike all the other systems. Not the prettiest but not too bad either.
			logger::LOGGER.Initialize(true);

			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Initializing tool.");

			// We initialize the window first and set the size and title after it has been created.
			m_Window.Initialize(true, a_hInstance);
			m_Window.SetTitle(a_sName);

			const glm::ivec2 size = m_Window.GetRealSize();
			m_DX12.Initialize(true, m_Window.GetHWnd(), size, &m_Window);

			m_ECS.Initialize();

			System::Initialize();

			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Initialized tool.");

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
		bool Tool::Destroy()
		{
			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Destroying engine.");

			m_ECS.Destroy();

			m_DX12.Destroy();

			m_Window.Destroy();

			// Destroy the logger last so we can see possible error messages from other systems.
			logger::LOGGER.Destroy();

			return System::Destroy();
		}

		//---------------------------------------------------------------------
		ResourceAtlas& Tool::GetResourceAtlas()
		{
			return m_ResourceAtlas;
		}

		//---------------------------------------------------------------------
		graphics::win32::Window& Tool::GetWindow()
		{
			return m_Window;
		}

		//---------------------------------------------------------------------
		graphics::dx12::DX12System2D& Tool::GetDX12()
		{
			return m_DX12;
		}

		//---------------------------------------------------------------------
		gameplay::EntityComponentSystem& Tool::GetECS()
		{
			return m_ECS;
		}
	}
}