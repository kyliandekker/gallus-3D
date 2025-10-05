#pragma once

#include "core/System.h"

// gameplay includes
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

			bool IsStarted() const
			{
				return m_bStarted;
			}

			void SetIsStarted(bool a_bStarted)
			{
				m_bStarted = a_bStarted;
			}

			bool IsPaused() const
			{
				return m_bPaused;
			}

			void SetIsPaused(bool a_bPaused)
			{
				m_bPaused = a_bPaused;
			}

			float GetFps() const
			{
				return m_Fps;
			}
		private:
			/// <summary>
			/// Callback for closing the window.
			/// </summary>
			void Shutdown();

			// There can only be one scene and setting a new one cleans up the old one automatically.
			gallus::gameplay::Scene m_Scene;

			bool m_bStarted = false;
			bool m_bPaused = false;

			float m_Fps;
		};
		inline extern Game GAME = {};
	}
}