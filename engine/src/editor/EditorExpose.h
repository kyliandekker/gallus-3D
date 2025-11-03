#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include "resources/AssetType.h"

namespace gallus
{
	enum class EditorWidgetType
	{
		None,
		DragFloat,
		DragInt8,
		DragInt16,
		DragInt32,
		DragInt64,
		Checkbox,
		Toggle,
		InputText,
		Vector2Field,
		AssetPicker,
		Object
	};

	// Field options with default values
	struct FieldOptions
	{
		EditorWidgetType type = EditorWidgetType::None;
		resources::AssetType assetType;
		std::string min = "";
		std::string max = "";
	};

	// Editor field with direct memory access
	struct EditorFieldInfo
	{
		const char* m_sName;
		size_t m_iOffset;
		const char* m_sUIName;
		FieldOptions m_Options;
	};

	// Interface
	class IExposableToEditor
	{
	public:
		virtual const std::vector<EditorFieldInfo>& GetEditorFields() const = 0;
		virtual const char* GetTypeName() const = 0;
		virtual ~IExposableToEditor() = default;
	};

	// Macros
	#define BEGIN_EXPOSED_FIELDS(CLASSNAME) \
	public: \
		static const std::vector<EditorFieldInfo>& StaticEditorFields() \
		{ \
			static std::vector<EditorFieldInfo> fields = {

	#define EXPOSE_FIELD(CLASSNAME, VAR, UINAME, ...) \
		{ #VAR, offsetof(CLASSNAME, VAR), UINAME, __VA_ARGS__ },

	#define END_EXPOSED_FIELDS(CLASSNAME) \
			}; \
			return fields; \
		} \
	const std::vector<EditorFieldInfo>& GetEditorFields() const override { return CLASSNAME::StaticEditorFields(); } \
	const char* GetTypeName() const override { return #CLASSNAME; }
}