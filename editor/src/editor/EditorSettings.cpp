#include "EditorSettings.h"

// external
#include <string>

namespace gallus
{
	namespace editor
	{
		//---------------------------------------------------------------------
		// Settings
		//---------------------------------------------------------------------
		void EditorSettings::SetScrollToBottom(bool a_bScrollToBottom)
		{
			m_bScrollToBottom = a_bScrollToBottom;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetScrollToBottom() const
		{
			return m_bScrollToBottom;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetShowInfo(bool a_bShowInfo)
		{
			m_bShowInfo = a_bShowInfo;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetShowInfo() const
		{
			return m_bShowInfo;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetShowTest(bool a_bShowText)
		{
			m_bShowTest = a_bShowText;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetShowTest() const
		{
			return m_bShowTest;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetShowWarning(bool a_bShowWarning)
		{
			m_bShowWarning = a_bShowWarning;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetShowWarning() const
		{
			return m_bShowWarning;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetShowError(bool a_bShowError)
		{
			m_bShowError = a_bShowError;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetShowError() const
		{
			return m_bShowError;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetShowAssert(bool a_bShowAssert)
		{
			m_bShowAssert = a_bShowAssert;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetShowAssert() const
		{
			return m_bShowAssert;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetShowSuccess(bool a_bShowSuccess)
		{
			m_bShowSuccess = a_bShowSuccess;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetShowSuccess() const
		{
			return m_bShowSuccess;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetShowInfoSuccess(bool a_bShowInfoSuccess)
		{
			m_bShowInfoSuccess = a_bShowInfoSuccess;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetShowInfoSuccess() const
		{
			return m_bShowInfoSuccess;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetShowAwesome(bool a_bShowAwesome)
		{
			m_bShowAwesome = a_bShowAwesome;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetShowAwesome() const
		{
			return m_bShowAwesome;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetShowGrid(bool a_bShowGrid)
		{
			m_bShowGrid = a_bShowGrid;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetShowGrid() const
		{
			return m_bShowGrid;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetFullScreenPlayMode(bool a_bFullScreenPlayMode)
		{
			m_bFullScreenPlayMode = a_bFullScreenPlayMode;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetFullScreenPlayMode() const
		{
			return m_bFullScreenPlayMode;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetSceneZoom(float a_fSceneZoom)
		{
			m_fSceneZoom = a_fSceneZoom;
		}

		//---------------------------------------------------------------------
		float EditorSettings::GetSceneZoom() const
		{
			return m_fSceneZoom;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetScenePanOffset(const Vector2& a_vScenePanOffset)
		{
			m_vScenePanOffset = a_vScenePanOffset;
		}

		//---------------------------------------------------------------------
		const Vector2& EditorSettings::GetScenePanOffset() const
		{
			return m_vScenePanOffset;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetLastSceneOperation(uint16_t a_iLastSceneOperation)
		{
			m_iLastSceneOperation = a_iLastSceneOperation;
		}

		//---------------------------------------------------------------------
		uint16_t EditorSettings::GetLastSceneOperation() const
		{
			return m_iLastSceneOperation;
		}
		
		//---------------------------------------------------------------------
		void EditorSettings::SetExplorerViewMode(uint8_t a_iExplorerViewMode)
		{
			m_iExplorerViewMode = a_iExplorerViewMode;
		}
		
		//---------------------------------------------------------------------
		uint8_t EditorSettings::GetExplorerViewMode() const
		{
			return m_iExplorerViewMode;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetFPSPrecision(bool a_bFPSPrecision)
		{
			m_bFPSPrecision = a_bFPSPrecision;
		}

		//---------------------------------------------------------------------
		bool EditorSettings::GetFPSPrecision() const
		{
			return m_bFPSPrecision;
		}

		//---------------------------------------------------------------------
		void EditorSettings::AddSceneToRecentScenes(const std::string& a_sName)
		{
			for (const OpenedScene& name : m_aLastOpenedScenes)
			{
				if (a_sName == name.name)
				{
					return;
				}
			}
			m_aLastOpenedScenes.emplace_back(a_sName);
		}

		//---------------------------------------------------------------------
		const std::vector<OpenedScene>& EditorSettings::GetLastOpenedScenes() const
		{
			return m_aLastOpenedScenes;
		}

		//---------------------------------------------------------------------
        void EditorSettings::SetDimensionDrawMode(uint8_t a_iDimensionDrawMode)
        {
			m_iDimensionDrawMode = a_iDimensionDrawMode;
		}

		//---------------------------------------------------------------------
		uint8_t EditorSettings::GetDimensionDrawMode() const
		{
			return m_iDimensionDrawMode;
		}

		//---------------------------------------------------------------------
		void EditorSettings::SetShadingDrawMode(uint8_t a_iShadingDrawMode)
		{
			m_iShadingDrawMode = a_iShadingDrawMode;
		}

		//---------------------------------------------------------------------
		uint8_t EditorSettings::GetShadingDrawMode() const
		{
			return m_iShadingDrawMode;
		}
	}
}