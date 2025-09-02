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
			Game() = default;

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
		private:
			/// <summary>
			/// Callback for closing the window.
			/// </summary>
			void Shutdown();

			/// <summary>
			/// Loads all textures.
			/// </summary>
			bool LoadTextures();

			/// <summary>
			/// Loads all sounds.
			/// </summary>
			bool LoadSounds();

			/// <summary>
			/// Testing purposes.
			/// </summary>
			bool Test();

			// There can only be one scene and setting a new one cleans up the old one automatically.
			gallus::gameplay::Scene m_Scene;

			bool m_bStarted = false;
		};
		extern inline Game GAME = {};
	}
}