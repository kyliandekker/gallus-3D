#pragma once

#include "core/System.h"

#include <string>

#include "core/Observable.h"

#include "graphics/imgui/views/inspector/InspectorView.h"
#include "graphics/dx12/Camera.h"

#include "editor/EditorSettings.h"
#include "editor/AssetDatabase.h"

#include "gameplay/Scene.h"
#include "gameplay/Prefab.h"

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
		/// Editor class that contains all editor-related data such as currently selected item, asset database, preview texture, etc.
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
			/// Retrieves the currently selected inspector view item.
			/// </summary>
			/// <returns>Pointer to the current selectable (can be null).</returns>
			const graphics::imgui::InspectorView* GetInspectorView() const
			{
				return m_pInspectorView;
			}

			/// <summary>
			/// Sets the selectable and inspector view.
			/// </summary>
			/// <param name="a_pSelectable">.The new selectable (can be null).</param>
			/// <param name="a_pInspectorView">The new inspector view (can be null).</param>
			void SetSelectable(graphics::imgui::EditorSelectable* a_pSelectable, graphics::imgui::InspectorView* a_pInspectorView)
			{
				if (m_pInspectorView)
				{
					delete m_pInspectorView;
					m_pInspectorView = nullptr;
				}
				m_pSelectable = a_pSelectable;
				m_pInspectorView = a_pInspectorView;
			}

			/// <summary>
			/// Retrieves the currently inspector view.
			/// </summary>
			/// <returns>Pointer to the current inspector view (can be null).</returns>
			graphics::imgui::InspectorView* GetInspectorView()
			{
				return m_pInspectorView;
			}

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
		protected:
			// TODO: Use m_bLoadAssets or something in AssetDatabase to wake up thread and let it sleep otherwise.
			bool Sleep() const override
			{
				return true;
			}
		private:
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
			graphics::imgui::InspectorView* m_pInspectorView = nullptr;

			gameplay::Scene m_CurrentScene;
			gameplay::Prefab m_Prefab;

			CameraMode m_CameraMode = CameraMode::CAMERA_MODE_SCENE;
			graphics::dx12::Camera m_EditorCamera;

			EditorMethod m_EditorMethod = EditorMethod::EDITOR_METHOD_SCENE;
		};
	}
}