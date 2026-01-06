#pragma once

// external
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>

namespace gallus
{
	class FieldSerializationInfo;
	class ISerializableObject;

	namespace graphics
	{
		namespace dx12
		{
			class Texture;
		}
		namespace imgui
		{
			bool ShowTexturePreview(const std::string& a_sId, graphics::dx12::Texture* a_pTexture, int a_iSpriteIndex = 0, const ImVec2& a_vSize = ImVec2(100, 100));
			bool ShowEditorFieldFromObject(ISerializableObject* a_pObject, const FieldSerializationInfo& a_Field, bool a_bInternal);
			bool RenderObjectFields(ISerializableObject* a_pObject, bool a_bInternal = false);
			bool RenderObjectGizmos(const ImRect& a_SceneViewRect, ISerializableObject* a_pObject);
		}
	}
}