#pragma once

#include "core/System.h"

// external
#include <string>

// core
#include "core/Observable.h"

// graphics
#include "graphics/dx12/Camera.h"

// gameplay
#include "gameplay/Scene.h"
#include "gameplay/Prefab.h"

// editor
#include "editor/EditorSettings.h"
#include "editor/AssetDatabase.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class EditorSelectable;
		}
	}
	namespace editor
	{
		enum class EditorMethod
		{
			EDITOR_METHOD_SCENE,
			EDITOR_METHOD_PREFAB
		};

		enum CameraMode
		{
			CAMERA_MODE_SCENE,
			CAMERA_MODE_GAME
		};

		//---------------------------------------------------------------------
		// Editor
		//---------------------------------------------------------------------
		/// <summary>
		/// Editor class that contains all editor-related data such as currently selected item, asset database, etc.
		/// </summary>
		class Editor : public core::ThreadedSystem
		{
		public:
			/// <summary>
			/// Constructs a window.
			/// </summary>
			Editor();

			/// <summary>
			/// Initializes the system, setting up necessary resources.
			/// </summary>
			/// <param name="a_bWait">Determines whether the application waits until the system has been fully initialized.</param>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			bool Initialize(bool a_bWait);

			/// <summary>
			/// Loop method for the thread.
			/// </summary>
			void Loop() override;

			/// <summary>
			/// Signals the thread to stop.
			/// </summary>
			bool Destroy() override;

			/// <summary>
			/// Retrieves the editor settings.
			/// </summary>
			/// <returns>Reference to the editor settings.</returns>
			editor::EditorSettings& GetEditorSettings()
			{
				return m_EditorSettings;
			}

			/// <summary>
			/// Retrieves the asset database.
			/// </summary>
			/// <returns>Reference to the asset database.</returns>
			editor::AssetDatabase& GetAssetDatabase()
			{
				return m_AssetDatabase;
			}

			/// <summary>
			/// Retrieves the currently selected item.
			/// </summary>
			/// <returns>Pointer to the current selectable (can be null).</returns>
			const core::Observable<graphics::imgui::EditorSelectable*>& GetSelectable() const
			{
				return m_pSelectable;
			}

			/// <summary>
			/// Sets the selectable and inspector view.
			/// </summary>
			/// <param name="a_pSelectable">.The new selectable (can be null).</param>
			void SetSelectable(graphics::imgui::EditorSelectable* a_pSelectable);

			std::mutex m_EditorMutex;

			/// <summary>
			/// Retrieves the scene.
			/// </summary>
			/// <returns>Returns a reference to the scene.</returns>
			gallus::gameplay::Scene& GetScene()
			{
				if (m_EditorMethod == EditorMethod::EDITOR_METHOD_SCENE)
				{
					return m_CurrentScene;
				}
				return m_Prefab;
			}

			/// <summary>
			/// Retrieves the editor method.
			/// </summary>
			/// <returns>Returns the type of editor method, scene or prefab.</returns>
			EditorMethod GetEditorMethod() const
			{
				return m_EditorMethod;
			}

			/// <summary>
			/// Sets the editor method.
			/// </summary>
			/// <param name="a_EditorMethod">.The editor method.</param>
			void SetEditorMethod(EditorMethod a_EditorMethod)
			{
				m_EditorMethod = a_EditorMethod;
			}

			/// <summary>
			/// Retrieves the editor camera.
			/// </summary>
			/// <returns>Returns a reference to the camera.</returns>
			graphics::dx12::Camera& GetEditorCamera()
			{
				return m_EditorCamera;
			}

			/// <summary>
			/// Retrieves the camera mode.
			/// </summary>
			/// <returns>Returns the camera mode, editor camera or game camera.</returns>
			CameraMode GetCameraMode() const
			{
				return m_CameraMode;
			}

			/// <summary>
			/// Sets the camera mode.
			/// </summary>
			void SetCameraMode(CameraMode a_CameraMode)
			{
				m_CameraMode = a_CameraMode;
			}

			const core::Observable<bool>& IsAnyDirty() const
			{
				return m_bIsAnyDirty;
			}

			void CopyGameFoV();
		protected:
			// TODO: Use m_bLoadAssets or something in AssetDatabase to wake up thread and let it sleep otherwise.
			bool Sleep() const override
			{
				return true;
			}
		private:
			core::Observable<bool> m_bIsAnyDirty;

			void SceneOrPrefabDirtyChanged(bool a_bPrev, bool a_bNew);
			
			/// <summary>
			/// Called once on the thread to perform initialization steps.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			bool InitThreadWorker() override;

			/// <summary>
			/// Destroys the system, releasing resources and performing necessary cleanup.
			/// </summary>
			/// <returns>True if the destruction was successful, otherwise false.</returns>
			void Finalize() override;

			editor::EditorSettings m_EditorSettings;
			editor::AssetDatabase m_AssetDatabase;

			core::Observable<graphics::imgui::EditorSelectable*> m_pSelectable;

			gameplay::Scene m_CurrentScene;
			gameplay::Prefab m_Prefab;

			CameraMode m_CameraMode = CameraMode::CAMERA_MODE_SCENE;
			graphics::dx12::Camera m_EditorCamera;

			EditorMethod m_EditorMethod = EditorMethod::EDITOR_METHOD_SCENE;
		};
	}
}