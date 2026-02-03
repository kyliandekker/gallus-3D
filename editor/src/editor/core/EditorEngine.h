#pragma once

// core
#include "core/Engine.h"

// editor
#include "editor/Editor.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// EditorEngine
		//---------------------------------------------------------------------
		class EditorEngine : public Engine
		{
		public:
			EditorEngine();

			~EditorEngine();

			/// <summary>
			/// Initializes the engine and all necessary subsystems with the specified parameters.
			/// </summary>
			/// <param name="a_hInstance">Handle to the program instance.</param>
			/// <param name="a_sName">Name of the program and window.</param>
			/// <returns>True if the engine initializes successfully, otherwise false.</returns>
			bool Initialize(HINSTANCE a_hInstance, const std::string& a_sName) override;

			/// <summary>
			/// Shuts down the engine and cleans up all subsystems.
			/// </summary>
			/// <returns>True if the destruction is successful, otherwise false.</returns>
			bool Destroy() override;

			/// <summary>
			/// Retrieves the editor.
			/// </summary>
			/// <returns>Reference to the editor.</returns>
			editor::Editor& GetEditor()
			{
				return m_Editor;
			}
		private:
			editor::Editor m_Editor;
		};
		extern inline EditorEngine* EDITOR_ENGINE = nullptr;
	}
}