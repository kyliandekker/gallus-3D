#pragma once

#include "System.h"
#include "engineEntryPoints.h"

// external
#include <wtypes.h>

namespace gallus::logger
{
	class Logger;
}
namespace gallus::graphics::dx12
{
	class DX12System;
}
namespace gallus::graphics::win32
{
	class Window;
}
namespace gallus::input
{
	class InputSystem;
}
namespace gallus::gameplay
{
	class EntityComponentSystem;
}
namespace gallus::resources
{
	class ResourceAtlas;
}
namespace gallus::core
{
	//---------------------------------------------------------------------
	// Engine
	//---------------------------------------------------------------------
	/// <summary>
	/// Main tool that manages all systems in the program, like initialization, startup, update and shutdown.
	/// </summary>
	class Engine : public System
	{
	public:
		/// <summary>
		/// Constructs the engine.
		/// </summary>
		Engine();

		virtual ~Engine();

		/// <summary>
		/// Initializes the engine and all necessary subsystems with the specified parameters.
		/// </summary>
		/// <param name="a_hInstance">Handle to the program instance.</param>
		/// <param name="a_sName">Name of the program and window.</param>
		/// <returns>True if the engine initializes successfully, otherwise false.</returns>
		virtual bool Initialize(HINSTANCE a_hInstance, const std::string& a_sName);

		/// <summary>
		/// Shuts down the engine and cleans up all subsystems.
		/// </summary>
		/// <returns>True if the destruction is successful, otherwise false.</returns>
		virtual bool Destroy() override;

		/// <summary>
		/// Retrieves the logger.
		/// </summary>
		/// <returns>Pointer to the logger.</returns>
		logger::Logger* GetLogger();

		/// <summary>
		/// Retrieves the resource atlas.
		/// </summary>
		/// <returns>Pointer to the resource atlas.</returns>
		resources::ResourceAtlas* GetResourceAtlas();

		/// <summary>
		/// Retrieves the window.
		/// </summary>
		/// <returns>Pointer to the window.</returns>
		graphics::win32::Window* GetWindow();

		/// <summary>
		/// Retrieves the dx12 system.
		/// </summary>
		/// <returns>Pointer to the dx12 system.</returns>
		graphics::dx12::DX12System* GetDX12();

		/// <summary>
		/// Retrieves the input system.
		/// </summary>
		/// <returns>Pointer to the input system.</returns>
		input::InputSystem* GetInputSystem();

		/// <summary>
		/// Retrieves the ecs.
		/// </summary>
		/// <returns>Pointer to the ecs.</returns>
		gameplay::EntityComponentSystem* GetECS();

		/// <summary>
		/// Sets the default arguments of the application.
		/// </summary>
		virtual void SetDefaultArguments() const;
	private:
		std::unique_ptr<logger::Logger> m_pLogger = nullptr;
		std::unique_ptr<resources::ResourceAtlas> m_pResourceAtlas = nullptr;
		std::unique_ptr<graphics::win32::Window> m_pWindow = nullptr;
		std::unique_ptr<graphics::dx12::DX12System> m_pDX12 = nullptr;
		std::unique_ptr<input::InputSystem> m_pInputSystem = nullptr;
		std::unique_ptr<gameplay::EntityComponentSystem> m_pECS = nullptr;
	};
}