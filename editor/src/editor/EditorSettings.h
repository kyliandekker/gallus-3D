#pragma once

#include "core/Settings.h"

#include "utils/math.h"

namespace gallus
{
	namespace editor
	{
		class OpenedScene : public ISerializableObject
		{
		public:
			OpenedScene() : ISerializableObject()
			{}
			OpenedScene(const std::string& a_sName) : ISerializableObject(),
				name(a_sName)
			{}

			std::string name;

			BEGIN_SERIALIZE(OpenedScene)
				SERIALIZE_FIELD(name, "name", "", .type = gallus::FieldSerializationType::FieldSerializationType_String)
			END_SERIALIZE(OpenedScene)
		};

		//---------------------------------------------------------------------
		// EditorSettings
		//---------------------------------------------------------------------
		class EditorSettings : public core::Settings
		{
		public:
			/// <summary>
			/// Constructs settings with file name.
			/// </summary>
			/// <param name="a_sFileName">The name the settings file should have.</param>
			EditorSettings(const std::string& a_sFileName) : core::Settings(a_sFileName)
			{}

			/// <summary>
			/// Sets whether the console should automatically scroll to the bottom.
			/// </summary>
			/// <param name="a_bScrollToBottom">True to enable auto-scrolling, otherwise false.</param>
			void SetScrollToBottom(bool a_bScrollToBottom);

			/// <summary>
			/// Checks if the console is set to auto-scroll to the bottom.
			/// </summary>
			/// <returns>True if auto-scrolling is enabled, otherwise false.</returns>
			bool GetScrollToBottom() const;

			/// <summary>
			/// Sets whether info log messages are displayed.
			/// </summary>
			/// <param name="a_bShowInfo">True to show info messages, otherwise false.</param>
			void SetShowInfo(bool a_bShowInfo);

			/// <summary>
			/// Checks if info log messages are displayed.
			/// </summary>
			/// <returns>True if info messages are visible, otherwise false.</returns>
			bool GetShowInfo() const;

			/// <summary>
			/// Sets whether test log messages are displayed.
			/// </summary>
			/// <param name="a_bShowText">True to show test messages, otherwise false.</param>
			void SetShowTest(bool a_bShowText);

			/// <summary>
			/// Checks if test log messages are displayed.
			/// </summary>
			/// <returns>True if test messages are visible, otherwise false.</returns>
			bool GetShowTest() const;

			/// <summary>
			/// Sets whether warning log messages are displayed.
			/// </summary>
			/// <param name="a_Warning">True to show warnings, otherwise false.</param>
			void SetShowWarning(bool a_Warning);

			/// <summary>
			/// Checks if warning log messages are displayed.
			/// </summary>
			/// <returns>True if warnings are visible, otherwise false.</returns>
			bool GetShowWarning() const;

			/// <summary>
			/// Sets whether error log messages are displayed.
			/// </summary>
			/// <param name="a_bShowError">True to show errors, otherwise false.</param>
			void SetShowError(bool a_bShowError);

			/// <summary>
			/// Checks if error log messages are displayed.
			/// </summary>
			/// <returns>True if errors are visible, otherwise false.</returns>
			bool GetShowError() const;

			/// <summary>
			/// Sets whether assertion log messages are displayed.
			/// </summary>
			/// <param name="a_bShowAssert">True to show assertion messages, otherwise false.</param>
			void SetShowAssert(bool a_bShowAssert);

			/// <summary>
			/// Checks if assertion log messages are displayed.
			/// </summary>
			/// <returns>True if assertions are visible, otherwise false.</returns>
			bool GetShowAssert() const;

			/// <summary>
			/// Sets whether success log messages are displayed.
			/// </summary>
			/// <param name="a_bShowSuccess">True to show success messages, otherwise false.</param>
			void SetShowSuccess(bool a_bShowSuccess);

			/// <summary>
			/// Checks if success log messages are displayed.
			/// </summary>
			/// <returns>True if success messages are visible, otherwise false.</returns>
			bool GetShowSuccess() const;

			/// <summary>
			/// Sets whether combined info and success log messages are displayed.
			/// </summary>
			/// <param name="a_bShowInfoSuccess">True to show combined info-success messages, otherwise false.</param>
			void SetShowInfoSuccess(bool a_bShowInfoSuccess);

			/// <summary>
			/// Checks if combined info and success log messages are displayed.
			/// </summary>
			/// <returns>True if combined info-success messages are visible, otherwise false.</returns>
			bool GetShowInfoSuccess() const;

			/// <summary>
			/// Sets whether "awesome" log messages are displayed.
			/// </summary>
			/// <param name="a_bShowAwesome">True to show "awesome" messages, otherwise false.</param>
			void SetShowAwesome(bool a_bShowAwesome);

			/// <summary>
			/// Checks if "awesome" log messages are displayed.
			/// </summary>
			/// <returns>True if "awesome" messages are visible, otherwise false.</returns>
			bool GetShowAwesome() const;

			/// <summary>
			/// Sets whether the grid should be shown.
			/// </summary>
			/// <param name="a_bShowGrid">True to show the grid, false otherwise.</param>
			void SetShowGrid(bool a_bShowGrid);

			/// <summary>
			/// Checks if the grid should be shown.
			/// </summary>
			/// <returns>True if grid is shown, false otherwise.</param>
			bool GetShowGrid() const;

			/// <summary>
			/// Sets whether play mode should open in full screen.
			/// </summary>
			/// <param name="a_bFullScreenPlayMode">True to set play mode to full screen, false otherwise.</param>
			void SetFullScreenPlayMode(bool a_bFullScreenPlayMode);

			/// <summary>
			/// Checks if play mode is in full screen.
			/// </summary>
			/// <returns>True if play mode is set to full screen, false otherwise.</param>
			bool GetFullScreenPlayMode() const;

			/// <summary>
			/// Sets the scene zoom.
			/// </summary>
			/// <param name="a_fSceneZoom">The amount of zoom.</param>
			void SetSceneZoom(float a_fSceneZoom);

			/// <summary>
			/// Determines how much the scene window is zoomed in.
			/// </summary>
			/// <returns>Float representing the zoom.</returns>
			float GetSceneZoom() const;

			/// <summary>
			/// Sets the scene pan offset.
			/// </summary>
			/// <param name="a_vScenePanOffset">The offset of the scene window.</param>
			void SetScenePanOffset(const Vector2& a_vScenePanOffset);

			/// <summary>
			/// Determines the offset of the scene window.
			/// </summary>
			/// <returns>Vector representing the offset.</returns>
			const Vector2& GetScenePanOffset() const;

			/// <summary>
			/// Sets the scene operation.
			/// </summary>
			/// <param name="a_iLastSceneOperation">The index of the scene operation.</param>
			void SetLastSceneOperation(int a_iLastSceneOperation);

			/// <summary>
			/// Determines the operation used in the scene for object transformation.
			/// </summary>
			/// <returns>Integer representing the scene operation index.</returns>
			int GetLastSceneOperation() const;

			/// <summary>
			/// Sets the view mode in explorer (list, grid, etc).
			/// </summary>
			/// <param name="a_iExplorerViewMode">The index of the grid mode.</param>
			void SetExplorerViewMode(int a_iExplorerViewMode);

			/// <summary>
			/// Determines the view mode in the explorer (list, grid, etc).
			/// </summary>
			/// <returns>Integer representing the view mode.</returns>
			int GetExplorerViewMode() const;

			/// <summary>
			/// Sets whether FPS should be shown in decimals.
			/// </summary>
			/// <param name="a_bFPSPrecision">True to round up FPS numbers, false to show full numbers.</param>
			void SetFPSPrecision(bool a_bFPSPrecision);

			/// <summary>
			/// Determines whether FPS should be shown in decimals.
			/// </summary>
			/// <returns>True if FPS numbers are rounded up, false otherwise.</param>
			bool GetFPSPrecision() const;

			void AddSceneToRecentScenes(const std::string& a_sName);

			const std::vector<OpenedScene>& GetLastOpenedScenes() const;
		private:
			bool m_bScrollToBottom = false; /// Auto-scroll setting for the console.
			bool m_bShowInfo = true; /// Check for info log messages.
			bool m_bShowTest = true; /// Check for test log messages.
			bool m_bShowWarning = true; /// Check for warning log messages.
			bool m_bShowError = true; /// Check for error log messages.
			bool m_bShowAssert = true; /// Check for assertion log messages.
			bool m_bShowSuccess = true; /// Check for success log messages.
			bool m_bShowInfoSuccess = true; /// Check for combined info-success messages.
			bool m_bShowAwesome = true; /// Check for "awesome" log messages.

			bool m_bShowGrid = false;
			bool m_bFullScreenPlayMode = false;
			float m_fSceneZoom = 1.0f;
			Vector2 m_vScenePanOffset = Vector2(0.0f, 0.0f);
			int m_iLastSceneOperation = 7;

			int m_iExplorerViewMode = 0;

			bool m_bFPSPrecision = false;

			std::vector<OpenedScene> m_aLastOpenedScenes;

			BEGIN_SERIALIZE_PARENT(EditorSettings, Settings)
				SERIALIZE_FIELD(m_bScrollToBottom, "Scroll To Bottom", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)
				SERIALIZE_FIELD(m_bShowInfo, "Show Info", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)
				SERIALIZE_FIELD(m_bShowTest, "Show Test", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)
				SERIALIZE_FIELD(m_bShowWarning, "Show Warning", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)
				SERIALIZE_FIELD(m_bShowError, "Show Error", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)
				SERIALIZE_FIELD(m_bShowAssert, "Show Assert", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)
				SERIALIZE_FIELD(m_bShowSuccess, "Show Success", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)
				SERIALIZE_FIELD(m_bShowInfoSuccess, "Show Info Success", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)
				SERIALIZE_FIELD(m_bShowAwesome, "Show Awesome", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)

				SERIALIZE_FIELD(m_bShowGrid, "Show Grid", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)
				SERIALIZE_FIELD(m_bFullScreenPlayMode, "Show Full Screen Play Mode", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)
				SERIALIZE_FIELD(m_fSceneZoom, "Scene Zoom", "",
					.type = FieldSerializationType::FieldSerializationType_Float)
				SERIALIZE_FIELD(m_vScenePanOffset, "Pan Offset", "",
					.type = FieldSerializationType::FieldSerializationType_Vector2)
				SERIALIZE_FIELD(m_iLastSceneOperation, "Last Scene Operation", "",
					.type = FieldSerializationType::FieldSerializationType_Int)

				SERIALIZE_FIELD(m_iExplorerViewMode, "Explorer View Mode", "",
					.type = FieldSerializationType::FieldSerializationType_Int)

				SERIALIZE_FIELD(m_bFPSPrecision, "Fps Precision", "",
					.type = FieldSerializationType::FieldSerializationType_Bool)

				SERIALIZE_FIELD_OPTIONS(m_aLastOpenedScenes, "Last Scenes Opened", "Saves the last opened scenes.", MakeArrayFieldSerializationOptions<OpenedScene>())
			END_SERIALIZE(Settings)
		};
	}
}