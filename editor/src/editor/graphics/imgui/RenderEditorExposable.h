#pragma once

namespace gallus
{
	class EditorFieldInfo;
	class IExposableToEditor;

	namespace graphics
	{
		namespace imgui
		{
			void ShowEditorFieldFromObject(IExposableToEditor* obj, const EditorFieldInfo& field);
			void RenderEditorForObject(IExposableToEditor* obj);
		}
	}
}