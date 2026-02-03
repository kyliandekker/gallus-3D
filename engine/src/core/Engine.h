#pragma once

#include "System.h"

// external
#include <wtypes.h>

// utils
#include "utils/file_abstractions.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class DX12System;
		}
		namespace win32
		{
			class Window;
		}
	}

	namespace input
	{
		class InputSystem;
	}
	namespace gameplay
	{
		class EntityComponentSystem;
	}
	namespace resources
	{
		class ResourceAtlas;
	}

	namespace core
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
			/// Retrieves the resource atlas.
			/// </summary>
			/// <returns>Reference to the resource atlas.</returns>
			resources::ResourceAtlas* GetResourceAtlas();

			/// <summary>
			/// Retrieves the window.
			/// </summary>
			/// <returns>Reference to the window.</returns>
			graphics::win32::Window* GetWindow();

			/// <summary>
			/// Retrieves the dx12 system.
			/// </summary>
			/// <returns>Reference to the dx12 system.</returns>
			graphics::dx12::DX12System* GetDX12();

			/// <summary>
			/// Retrieves the input system.
			/// </summary>
			/// <returns>Reference to the input system.</returns>
			input::InputSystem* GetInputSystem();

			/// <summary>
			/// Retrieves the ecs.
			/// </summary>
			/// <returns>Reference to the ecs.</returns>
			gameplay::EntityComponentSystem* GetECS();

			/// <summary>
			/// Retrieves the save directory of the application.
			/// </summary>
			/// <returns>Path to the save directory.</returns>
			const std::filesystem::path& GetSaveDirectory()
			{
				return m_sSaveDirectory;
			}

			/// <summary>
			/// Sets the save directory of the application.
			/// </summary>
			/// <param name="a_sSaveDirectory">Save directory.</param>
			void SetSaveDirectory(const std::filesystem::path& a_sSaveDirectory)
			{
				m_sSaveDirectory = a_sSaveDirectory;
				file::CreateDirectory(a_sSaveDirectory);
			}

			/// <summary>
			/// Sets the default arguments of the application.
			/// </summary>
			virtual void SetDefaultArguments() const;
		private:
			std::unique_ptr<resources::ResourceAtlas> m_pResourceAtlas = nullptr;
			std::unique_ptr<graphics::win32::Window> m_pWindow = nullptr;
			std::unique_ptr<graphics::dx12::DX12System> m_pDX12 = nullptr;
			std::unique_ptr<input::InputSystem> m_pInputSystem = nullptr;
			std::unique_ptr<gameplay::EntityComponentSystem> m_pECS = nullptr;

			std::filesystem::path m_sSaveDirectory;
		};
		extern inline Engine* ENGINE = nullptr;
	}
}