// header
#include "EditorSettings.h"

// external
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/utils.h>
#include <string>

#define JSON_EDITOR_SETTINGS_CONSOLE_VAR "console"
#define JSON_EDITOR_SETTINGS_CONSOLE_SCROLL_TO_BOTTOM_VAR "scrollToBottom"
#define JSON_EDITOR_SETTINGS_CONSOLE_INFO_VAR "info"
#define JSON_EDITOR_SETTINGS_CONSOLE_TEST_VAR "test"
#define JSON_EDITOR_SETTINGS_CONSOLE_WARNING_VAR "warning"
#define JSON_EDITOR_SETTINGS_CONSOLE_ERROR_VAR "error"
#define JSON_EDITOR_SETTINGS_CONSOLE_ASSERT_VAR "assert"
#define JSON_EDITOR_SETTINGS_CONSOLE_SUCCESS_VAR "success"
#define JSON_EDITOR_SETTINGS_CONSOLE_INFOSUCCESS_VAR "infoSuccess"
#define JSON_EDITOR_SETTINGS_CONSOLE_AWESOME_VAR "awesome"

#define JSON_EDITOR_SETTINGS_SCENE_VAR "scene"
#define JSON_EDITOR_SETTINGS_SCENE_ZOOM "zoom"
#define JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET "panOffset"
#define JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET_X "x"
#define JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET_Y "y"
#define JSON_EDITOR_SETTINGS_SCENE_LAST_OPERATION "lastOperation"
#define JSON_EDITOR_SETTINGS_SCENE_DRAW_BOUNDS "drawBounds"

namespace gallus
{
    namespace editor
    {
        //---------------------------------------------------------------------
        // Settings
        //---------------------------------------------------------------------
        bool EditorSettings::LoadVars(const rapidjson::Document& a_Document)
        {
            if (a_Document.HasMember(JSON_EDITOR_SETTINGS_CONSOLE_VAR) && a_Document[JSON_EDITOR_SETTINGS_CONSOLE_VAR].IsObject())
            {
                GetBool(a_Document[JSON_EDITOR_SETTINGS_CONSOLE_VAR], JSON_EDITOR_SETTINGS_CONSOLE_SCROLL_TO_BOTTOM_VAR, m_bScrollToBottom);

                GetBool(a_Document[JSON_EDITOR_SETTINGS_CONSOLE_VAR], JSON_EDITOR_SETTINGS_CONSOLE_ASSERT_VAR, m_bShowAssert);
                GetBool(a_Document[JSON_EDITOR_SETTINGS_CONSOLE_VAR], JSON_EDITOR_SETTINGS_CONSOLE_ERROR_VAR, m_bShowError);
                GetBool(a_Document[JSON_EDITOR_SETTINGS_CONSOLE_VAR], JSON_EDITOR_SETTINGS_CONSOLE_WARNING_VAR, m_bShowWarning);
                GetBool(a_Document[JSON_EDITOR_SETTINGS_CONSOLE_VAR], JSON_EDITOR_SETTINGS_CONSOLE_INFO_VAR, m_bShowInfo);
                GetBool(a_Document[JSON_EDITOR_SETTINGS_CONSOLE_VAR], JSON_EDITOR_SETTINGS_CONSOLE_TEST_VAR, m_bShowTest);
                GetBool(a_Document[JSON_EDITOR_SETTINGS_CONSOLE_VAR], JSON_EDITOR_SETTINGS_CONSOLE_SUCCESS_VAR, m_bShowSuccess);
                GetBool(a_Document[JSON_EDITOR_SETTINGS_CONSOLE_VAR], JSON_EDITOR_SETTINGS_CONSOLE_INFOSUCCESS_VAR, m_bShowInfoSuccess);
                GetBool(a_Document[JSON_EDITOR_SETTINGS_CONSOLE_VAR], JSON_EDITOR_SETTINGS_CONSOLE_AWESOME_VAR, m_bShowAwesome);
            }

            if (a_Document.HasMember(JSON_EDITOR_SETTINGS_SCENE_VAR) && a_Document[JSON_EDITOR_SETTINGS_SCENE_VAR].IsObject())
            {
                GetFloat(a_Document[JSON_EDITOR_SETTINGS_SCENE_VAR], JSON_EDITOR_SETTINGS_SCENE_ZOOM, m_fSceneZoom);
                if (a_Document[JSON_EDITOR_SETTINGS_SCENE_VAR].HasMember(JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET) && a_Document[JSON_EDITOR_SETTINGS_SCENE_VAR][JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET].IsObject())
                {
                    GetFloat(a_Document[JSON_EDITOR_SETTINGS_SCENE_VAR][JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET], JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET_X, m_vScenePanOffset.x);
                    GetFloat(a_Document[JSON_EDITOR_SETTINGS_SCENE_VAR][JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET], JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET_Y, m_vScenePanOffset.y);
                }
                GetInt(a_Document[JSON_EDITOR_SETTINGS_SCENE_VAR], JSON_EDITOR_SETTINGS_SCENE_LAST_OPERATION, m_iLastSceneOperation);
                GetBool(a_Document[JSON_EDITOR_SETTINGS_SCENE_VAR], JSON_EDITOR_SETTINGS_SCENE_DRAW_BOUNDS, m_bDrawBounds);
            }

            return true;
        }

        //---------------------------------------------------------------------
        bool EditorSettings::SaveVars(rapidjson::Document& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
        {
            rapidjson::Document consoleDoc;
            consoleDoc.SetObject();
            consoleDoc.AddMember(JSON_EDITOR_SETTINGS_CONSOLE_SCROLL_TO_BOTTOM_VAR, m_bScrollToBottom, a_Allocator);
            consoleDoc.AddMember(JSON_EDITOR_SETTINGS_CONSOLE_ASSERT_VAR, m_bShowAssert, a_Allocator);
            consoleDoc.AddMember(JSON_EDITOR_SETTINGS_CONSOLE_ERROR_VAR, m_bShowError, a_Allocator);
            consoleDoc.AddMember(JSON_EDITOR_SETTINGS_CONSOLE_WARNING_VAR, m_bShowWarning, a_Allocator);
            consoleDoc.AddMember(JSON_EDITOR_SETTINGS_CONSOLE_INFO_VAR, m_bShowInfo, a_Allocator);
            consoleDoc.AddMember(JSON_EDITOR_SETTINGS_CONSOLE_TEST_VAR, m_bShowTest, a_Allocator);
            consoleDoc.AddMember(JSON_EDITOR_SETTINGS_CONSOLE_SUCCESS_VAR, m_bShowSuccess, a_Allocator);
            consoleDoc.AddMember(JSON_EDITOR_SETTINGS_CONSOLE_INFOSUCCESS_VAR, m_bShowInfoSuccess, a_Allocator);
            consoleDoc.AddMember(JSON_EDITOR_SETTINGS_CONSOLE_AWESOME_VAR, m_bShowAwesome, a_Allocator);
            a_Document.AddMember(JSON_EDITOR_SETTINGS_CONSOLE_VAR, consoleDoc, a_Allocator);

            rapidjson::Document sceneDoc;
            sceneDoc.SetObject();
            sceneDoc.AddMember(JSON_EDITOR_SETTINGS_SCENE_ZOOM, m_fSceneZoom, a_Allocator);
            rapidjson::Document scenePanOffsetDoc;
            scenePanOffsetDoc.SetObject();
            scenePanOffsetDoc.AddMember(JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET_X, m_vScenePanOffset.x, a_Allocator);
            scenePanOffsetDoc.AddMember(JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET_Y, m_vScenePanOffset.y, a_Allocator);
            sceneDoc.AddMember(JSON_EDITOR_SETTINGS_SCENE_PAN_OFFSET, scenePanOffsetDoc, a_Allocator);
            sceneDoc.AddMember(JSON_EDITOR_SETTINGS_SCENE_LAST_OPERATION, m_iLastSceneOperation, a_Allocator);
            sceneDoc.AddMember(JSON_EDITOR_SETTINGS_SCENE_DRAW_BOUNDS, m_bDrawBounds, a_Allocator);
            a_Document.AddMember(JSON_EDITOR_SETTINGS_SCENE_VAR, sceneDoc, a_Allocator);

            return true;
        }

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
        void EditorSettings::SetScenePanOffset(const glm::vec2& a_vScenePanOffset)
        {
            m_vScenePanOffset = a_vScenePanOffset;
        }

        //---------------------------------------------------------------------
        const glm::vec2& EditorSettings::GetScenePanOffset() const
        {
            return m_vScenePanOffset;
        }

        //---------------------------------------------------------------------
        void EditorSettings::SetLastSceneOperation(int a_iLastSceneOperation)
        {
            m_iLastSceneOperation = a_iLastSceneOperation;
        }

        //---------------------------------------------------------------------
        int EditorSettings::GetLastSceneOperation() const
        {
            return m_iLastSceneOperation;
        }

        //---------------------------------------------------------------------
        void EditorSettings::SetDrawBounds(bool a_bDrawBounds)
        {
            m_bDrawBounds = a_bDrawBounds;
        }

        //---------------------------------------------------------------------
        bool EditorSettings::GetDrawBounds() const
        {
            return m_bDrawBounds;
        }
    }
}