#pragma once

#include "System.h"

#include <wtypes.h>

// core includes
#include "resources/ResourceAtlas.h"

// utils includes
#include "utils/file_abstractions.h"

// graphics includes
#include "graphics/dx12/DX12System2D.h"
#include "graphics/win32/Window.h"

// gameplay includes
#include "gameplay/EntityComponentSystem.h"

namespace gallus
{
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
			Engine() = default;

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
			resources::ResourceAtlas& GetResourceAtlas();

			/// <summary>
			/// Retrieves the window.
			/// </summary>
			/// <returns>Reference to the window.</returns>
			graphics::win32::Window& GetWindow();

			/// <summary>
			/// Retrieves the dx12 system.
			/// </summary>
			/// <returns>Reference to the dx12 system.</returns>
			graphics::dx12::DX12System2D& GetDX12();

			/// <summary>
			/// Retrieves the ecs.
			/// </summary>
			/// <returns>Reference to the ecs.</returns>
			gameplay::EntityComponentSystem& GetECS();

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
			resources::ResourceAtlas m_ResourceAtlas;
			graphics::win32::Window m_Window;
			graphics::dx12::DX12System2D m_DX12;
			gameplay::EntityComponentSystem m_ECS;

			std::filesystem::path m_sSaveDirectory;
		};
		extern inline Engine* ENGINE = nullptr;
	}
}