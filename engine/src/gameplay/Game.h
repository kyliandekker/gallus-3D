#pragma once

// base class
#include "core/System.h"

// core
#include "core/Observable.h"
#include "core/Event.h"

// graphics
#include "graphics/dx12/FPSCounter.h"

// gameplay
#include "gameplay/Scene.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Game
		//---------------------------------------------------------------------
		/// <summary>
		/// Game that handles all the specific entities, scenes, textures, etc.
		/// </summary>
		class Game : public gallus::core::System
		{
		public:
			/// <summary>
			/// Initializes the engine and all necessary subsystems with the specified parameters.
			/// </summary>
			/// <returns>True if the engine initializes successfully, otherwise false.</returns>
			virtual bool Initialize();

			/// <summary>
			/// Shuts down the engine and cleans up all subsystems.
			/// </summary>
			/// <returns>True if the destruction is successful, otherwise false.</returns>
			virtual bool Destroy() override;

			/// <summary>
			/// Main loop of the game.
			/// </summary>
			void Loop();

			/// <summary>
			/// Retrieves the scene.
			/// </summary>
			/// <returns>Returns a reference to the scene.</returns>
			gallus::gameplay::Scene& GetScene()
			{
				return m_Scene;
			}

			/// <summary>
			/// Retrieves the starting state.
			/// </summary>
			/// <returns>True if the game has started, false otherwise.</returns>
			bool IsStarted() const
			{
				return m_bStarted.get();
			}

			/// <summary>
			/// Retrieves the starting state.
			/// </summary>
			/// <returns>True if the game has started, false otherwise.</returns>
			const core::Observable<bool>& IsStartedObs() const
			{
				return m_bStarted;
			}

			/// <summary>
			/// Sets the starting state.
			/// </summary>
			/// <param name="a_bStarted">True if the game should be started, false otherwise.</param>
			void SetIsStarted(bool a_bStarted)
			{
				m_bStarted = a_bStarted;
			}

			/// <summary>
			/// Retrieves the pause state.
			/// </summary>
			/// <returns>True if the game is paused, false otherwise.</returns>
			bool IsPaused() const
			{
				return m_bPaused;
			}

			/// <summary>
			/// Sets the pause state.
			/// </summary>
			/// <param name="a_bStarted">True if the game should be paused, false otherwise.</param>
			void SetIsPaused(bool a_bPaused)
			{
				m_bPaused = a_bPaused;
			}

			const graphics::FPSCounter& GetFPS() const
			{
				return m_FpsCounter;
			}

			const Event<float>& OnNewFrame() const
			{
				return m_eOnNewFrame;
			}

			void NewFrame(float a_fDeltaTime);
		private:
			/// <summary>
			/// Callback for closing the window.
			/// </summary>
			void Shutdown();

			// There can only be one scene and setting a new one cleans up the old one automatically.
			gallus::gameplay::Scene m_Scene;
			Event<float> m_eOnNewFrame;

			core::Observable<bool> m_bStarted;
			bool m_bPaused = false;

			graphics::FPSCounter m_FpsCounter;
		};
		inline extern Game GAME = {};
	}
}