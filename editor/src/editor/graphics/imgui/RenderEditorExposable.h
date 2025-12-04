#pragma once

// external
#include <imgui/imgui.h>

namespace gallus
{
	class EditorFieldInfo;
	class IExposableToEditor;

	namespace graphics
	{
		namespace imgui
		{
			bool ShowEditorFieldFromObject(IExposableToEditor* a_pObject, const EditorFieldInfo& a_Field, bool a_bInternal);
			bool RenderObjectFields(IExposableToEditor* a_pObject);
			bool RenderObjectGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom, IExposableToEditor* a_pObject);
		}
	}
}