#pragma once

// external
#include <imgui/imgui.h>
#include <string>

#include "gameplay/EntityID.h"

struct ImRect;

namespace gallus
{
	class FieldSerializationInfo;
	class ISerializableObject;
}
namespace gallus::graphics::dx12
{
	class Texture;
}
namespace gallus::graphics::imgui
{
	bool ShowTexturePreview(const std::string& a_sId, graphics::dx12::Texture* a_pTexture, uint8_t a_iTextureIndex = 0, const ImVec2& a_vSize = {});
	bool ShowEditorFieldFromObject(ISerializableObject* a_pObject, const FieldSerializationInfo& a_Field, bool a_bInternal);
	bool RenderObjectFields(ISerializableObject* a_pObject, bool a_bInternal = false);
	bool RenderObjectGizmos(const ImRect& a_SceneViewRect, ISerializableObject* a_pObject, const gameplay::EntityID& a_EntityID = gameplay::EntityID());
}