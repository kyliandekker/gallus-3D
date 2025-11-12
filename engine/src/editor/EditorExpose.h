#pragma once

#include <string>
#include <vector>
#include <functional>
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
		AssetPickerPtr,
		Object,
		ObjectPtr,
		EnumDropdown,
		TexturePreview,
	};

	// Field options with default values
	struct FieldOptions
	{
		EditorWidgetType type = EditorWidgetType::None;
		resources::AssetType assetType;
		std::string min = "";
		std::string max = "";
		std::function<std::string(int)> enumToStringFunc; // returns string name for index

		size_t relatedIndexFieldOffset = 0;
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

	template<typename TEnum>
	inline std::function<std::string(int)> MakeEnumToStringFunc(std::string(*toStringFunc)(TEnum))
	{
		// returns a std::function that editor code will call with an int index
		return [toStringFunc](int value) -> std::string
		{
			// Cast the int to the enum type and forward to user's converter
			return toStringFunc(static_cast<TEnum>(value));
		};
	}

	// Macro that produces the FieldOptions aggregate inline (fits your EXPOSE_FIELD usage)
	#define EXPOSE_ENUM_FIELD_OPTIONS(EnumType, ToStringFunc) \
		FieldOptions{ .type = EditorWidgetType::EnumDropdown, .enumToStringFunc = MakeEnumToStringFunc<EnumType>(ToStringFunc) }

	// Convenience macro that assumes your to-string function is named EnumTypeToString
	#define EXPOSE_ENUM_FIELD_OPTIONS_AUTO(EnumType) \
		EXPOSE_ENUM_FIELD_OPTIONS(EnumType, EnumType##ToString)

	// Handy macro that hides the FieldOptions completely and expands to EXPOSE_FIELD for a single-line usage
	#define EXPOSE_ENUM_FIELD(Class, Var, UIName, EnumType, ToStringFunc) \
		EXPOSE_FIELD(Class, Var, UIName, EXPOSE_ENUM_FIELD_OPTIONS(EnumType, ToStringFunc))

	// Convenience auto macro using EnumTypeToString naming convention
	#define EXPOSE_ENUM_FIELD_AUTO(Class, Var, UIName, EnumType) \
		EXPOSE_ENUM_FIELD(Class, Var, UIName, EnumType, EnumType##ToString)
}