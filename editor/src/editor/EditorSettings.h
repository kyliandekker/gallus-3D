#pragma once

// base class
#include "core/Settings.h"

// external
#include <glm/vec2.hpp>

namespace gallus
{
	namespace editor
	{
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
			{ }

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
			void SetScenePanOffset(const glm::vec2& a_vScenePanOffset);

			/// <summary>
			/// Determines the offset of the scene window.
			/// </summary>
			/// <returns>Vector representing the offset.</returns>
			const glm::vec2& GetScenePanOffset() const;

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
			/// Sets whether bounds should be drawn.
			/// </summary>
			/// <param name="a_bDrawBounds">True if bounds should be shown, false otherwise.</param>
			void SetDrawBounds(bool a_bDrawBounds);

			/// <summary>
			/// Returns whether bounds should be drawn.
			/// </summary>
			/// <returns>True if bounds should be drawn, otherwise false.</returns>
			bool GetDrawBounds() const;
		private:
			/// <summary>
			/// Virtual method for loading specific vars.
			/// </summary>
			/// <returns>True if the settings were loaded successfully, otherwise false.</returns>
			bool LoadVars(const rapidjson::Document& a_Document) override;

			/// <summary>
			/// Virtual method for saving specific vars.
			/// </summary>
			/// <returns>True if the settings were saved successfully, otherwise false.</returns>
			bool SaveVars(rapidjson::Document& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;

			bool m_bScrollToBottom = false; /// Auto-scroll setting for the console.
			bool m_bShowInfo = true; /// Check for info log messages.
			bool m_bShowTest = true; /// Check for test log messages.
			bool m_bShowWarning = true; /// Check for warning log messages.
			bool m_bShowError = true; /// Check for error log messages.
			bool m_bShowAssert = true; /// Check for assertion log messages.
			bool m_bShowSuccess = true; /// Check for success log messages.
			bool m_bShowInfoSuccess = true; /// Check for combined info-success messages.
			bool m_bShowAwesome = true; /// Check for "awesome" log messages.

			bool m_bDrawBounds = true; /// Check for drawing bounds in the scene view.
			float m_fSceneZoom = 1.0f;
			glm::vec2 m_vScenePanOffset = glm::vec2(0.0f, 0.0f);
			int m_iLastSceneOperation = 7;
		};
	}
}