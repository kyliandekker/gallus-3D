#pragma once

// external
#include <imgui/imgui.h>
#include <string>

namespace gallus
{
	class EditorFieldInfo;
	class IExposableToEditor;

	namespace graphics
	{
		namespace dx12
		{
			class Texture;
		}
		namespace imgui
		{
			bool ShowTexturePreview(const std::string& a_sId, graphics::dx12::Texture* a_pTexture, int a_iSpriteIndex = 0, const ImVec2& a_vSize = ImVec2(100, 100));
			bool ShowEditorFieldFromObject(IExposableToEditor* a_pObject, const EditorFieldInfo& a_Field, bool a_bInternal);
			bool RenderObjectFields(IExposableToEditor* a_pObject, bool a_bInternal = false);
			bool RenderObjectGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom, IExposableToEditor* a_pObject);
		}
	}
}