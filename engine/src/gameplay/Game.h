#pragma once

// base class
#include "core/System.h"

// core
#ifndef _RELEASE
#include "core/Observable.h"
#endif
#include "core/Event.h"

// graphics
#include "graphics/dx12/FPSCounter.h"

// gameplay
#include "gameplay/Scene.h"

namespace gallus::gameplay
{
    class Game;
    extern Game& GetGame(); // getter

	//---------------------------------------------------------------------
	// Game
	//---------------------------------------------------------------------
	/// <summary>
	/// Game that handles all the specific entities, scenes, textures, etc.
	/// </summary>
	class Game : public core::System
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
		Scene& GetScene()
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
		
#ifndef _RELEASE
		/// <summary>
		/// Retrieves the starting state.
		/// </summary>
		/// <returns>True if the game has started, false otherwise.</returns>
		const core::Observable<bool>& IsStartedObs() const
		{
			return m_bStarted;
		}
#endif

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

		/// <summary>
		/// Retrieves the FPS of the game loop.
		/// </summary>
		/// <returns>Reference to the FPS class.</returns>
		const graphics::FPSCounter& GetFPS() const
		{
			return m_FpsCounter;
		}

		const Event<float>& OnNewFrame() const
		{
			return m_eOnNewFrame;
		}

		/// <summary>
		/// Called when a new frame starts.
		/// </summary>
		/// <param name="a_fDeltaTime">The time that has passed since last frame.</param>
		void NewFrame(float a_fDeltaTime);
	private:
		/// <summary>
		/// Callback for closing the window.
		/// </summary>
		void Shutdown();

		// There can only be one scene and setting a new one cleans up the old one automatically.
		Scene m_Scene;
		Event<float> m_eOnNewFrame;

#ifndef _RELEASE
		core::Observable<bool> m_bStarted;
#else
		bool m_bStarted;
#endif
		bool m_bPaused = false;

		graphics::FPSCounter m_FpsCounter;
	};
}