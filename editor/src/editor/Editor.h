#pragma once

#include "core/System.h"

// external
#include <memory>

// core
#include "core/Event.h"

// gameplay
#include "gameplay/Scene.h"

// editor
#include "editor/EditorSettings.h"
#include "editor/graphics/imgui/EditorCameraViewport.h"
#include "editor/CameraMode.h"

namespace gallus::graphics::dx12
{
	class Camera;
}
namespace gallus::graphics::imgui
{
	class EditorSelectable;
}
namespace gallus::editor
{
	//---------------------------------------------------------------------
	// Editor
	//---------------------------------------------------------------------
	/// <summary>
	/// Editor class that contains all editor-related data such as currently selected item, asset database, etc.
	/// </summary>
	class Editor : public core::System
	{
	public:
		/// <summary>
		/// Constructs the editor.
		/// </summary>
		Editor();

		/// <summary>
		/// Destroys the editor.
		/// </summary>
		~Editor();

		/// <summary>
		/// Initializes the system, setting up necessary resources.
		/// </summary>
		/// <returns>True if the initialization was successful, otherwise false.</returns>
		bool Initialize() override;

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
		/// Retrieves the editor camera.
		/// </summary>
		/// <returns>Reference to the editor camera.</returns>
		graphics::dx12::Camera& GetEditorCamera();

		graphics::imgui::EditorCameraViewport& GetEditorCameraViewport();

		/// <summary>
		/// Retrieves the camera mode.
		/// </summary>
		/// <returns>Current camera mode.</returns>
		CameraMode GetCameraMode() const
		{
			return m_CameraMode;
		}

		/// <summary>
		/// Sets the camera mode.
		/// </summary>
		/// <param name="a_CameraMode">.The new camera mode.</param>
		void SetCameraMode(CameraMode a_CameraMode)
		{
			m_CameraMode = a_CameraMode;
		}
		
		/// <summary>
		/// Retrieves the currently selected item.
		/// </summary>
		/// <returns>Pointer to the current selectable (can be null).</returns>
		std::weak_ptr<graphics::imgui::EditorSelectable>& GetSelectable()
		{
			return m_pSelectable;
		}

		/// <summary>
		/// Sets the selectable and inspector view.
		/// </summary>
		/// <param name="a_pSelectable">.The new selectable.</param>
		void SetSelectable(std::weak_ptr<graphics::imgui::EditorSelectable> a_pSelectable);

		/// <summary>
		/// Compares a selectable to the current selectable.
		/// </summary>
		/// <param name="a_pSelectable">.The selectable to be compared.</param>
		/// <returns>True if pointers were the same, false otherwise.</returns>
		bool IsSelectable(std::shared_ptr<graphics::imgui::EditorSelectable> a_pSelectable);

		/// <summary>
		/// Resets the selectable and inspector view.
		/// </summary>
		void ResetSelectable();

		///// <summary>
		///// Retrieves the scene.
		///// </summary>
		///// <returns>Returns a reference to the scene.</returns>
		gameplay::Scene& GetScene()
		{
			return m_Scene;
		}

		const Event<std::shared_ptr<graphics::imgui::EditorSelectable>, std::shared_ptr<graphics::imgui::EditorSelectable>>& OnSelectableChanged() const
		{
			return m_eOnSelectableChanged;
		}

		std::mutex m_EditorMutex;
	protected:
		Event<std::shared_ptr<graphics::imgui::EditorSelectable>, std::shared_ptr<graphics::imgui::EditorSelectable>> m_eOnSelectableChanged;

		EditorSettings m_EditorSettings;
		graphics::imgui::EditorCameraViewport m_EditorCameraViewport;

		gameplay::Scene m_Scene;
		
		// General editor functionality.
		CameraMode m_CameraMode = CameraMode::CAMERA_MODE_SCENE; // Whether the current rendering camera is editor or game camera.
		std::unique_ptr<graphics::dx12::Camera> m_pEditorCamera;

		std::weak_ptr<graphics::imgui::EditorSelectable> m_pSelectable;
	};
}