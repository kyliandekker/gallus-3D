#pragma once

#ifdef _EDITOR

// external
#include <string>
#include <vector>
#include <functional>
#include <cstddef>

// resources
#include "resources/AssetType.h"

namespace gallus
{
	enum class EditorFieldWidgetType
	{
		None,
		DragFloat,
		DragInt8,
		DragInt16,
		DragInt32,
		DragInt64,
		Checkbox,
		Toggle,
		Vector2Field,
		Vector3Field,
		Quaternion,
		AssetPickerPtr,
		Object,
		ObjectPtr,
		EnumDropdown,
		TexturePreview
	};

	enum class EditorGizmoType
	{
		None,
		Transform
	};

	// Field options with default values
	struct FieldOptions
	{
		EditorFieldWidgetType type = EditorFieldWidgetType::None;
		resources::AssetType assetType;
		std::string min = "";
		std::string max = "";
		std::function<std::string(int)> enumToStringFunc; // returns string name for index
		std::string description;
		bool disabled = false;
		bool internal = false;

		size_t relatedIndexFieldOffset = 0;
	};

	// Gizmo options with default values
	struct GizmoOptions
	{
		EditorGizmoType type = EditorGizmoType::None;
	};

	// Editor field with direct memory access
	struct EditorFieldInfo
	{
		const char* m_sName;
		size_t m_iOffset;
		const char* m_sUIName;
		FieldOptions m_Options;
	};

	// Forward-declare interface so EditorGizmoInfo can mention it (minimal change)
	class IExposableToEditor;

	struct EditorGizmoInfo
	{
		const char* m_sName;
		size_t m_iOffset;
		GizmoOptions m_Options;
	};

	// Interface (full definition)
	class IExposableToEditor
	{
	public:
		virtual const std::vector<EditorFieldInfo>& GetEditorFields() const = 0;
		virtual const std::vector<EditorGizmoInfo>& GetEditorGizmos() const = 0;
		virtual const char* GetTypeName() const = 0;
		virtual ~IExposableToEditor() = default;
	};

	namespace resources
	{
		class SrcData;
	}
	void DeserializeEditorExposable(IExposableToEditor* a_pObject, const resources::SrcData& a_SrcData);
	void SerializeEditorExposable(const IExposableToEditor* a_pObject, resources::SrcData& a_SrcData);

	// No parent
#define BEGIN_EXPOSE_FIELDS(CLASSNAME) \
    public: \
        static const std::vector<EditorFieldInfo>& StaticEditorFields() \
        { \
            static std::vector<EditorFieldInfo> fields = []() { \
                std::vector<EditorFieldInfo> f;


// With parent
#define BEGIN_EXPOSE_FIELDS_PARENT(CLASSNAME, PARENT) \
    public: \
        static const std::vector<EditorFieldInfo>& StaticEditorFields() \
        { \
            static std::vector<EditorFieldInfo> fields = []() { \
                std::vector<EditorFieldInfo> f; \
                const auto& parentFields = PARENT::StaticEditorFields(); \
                f.insert(f.end(), parentFields.begin(), parentFields.end());

#define EXPOSE_FIELD(CLASSNAME, VAR, UINAME, FIELD_OPTIONS) \
                f.push_back({ #VAR, offsetof(CLASSNAME, VAR), UINAME, FIELD_OPTIONS });

#define END_EXPOSE_FIELDS(CLASSNAME) \
                return f; \
            }(); \
            return fields; \
        }

	// Gizmos (fields)
#define BEGIN_EXPOSE_GIZMOS(CLASSNAME) \
	public: \
		static const std::vector<EditorGizmoInfo>& StaticEditorGizmos() \
		{ \
			static std::vector<EditorGizmoInfo> gizmos = {

#define EXPOSE_GIZMO(CLASSNAME, VAR, GIZMO_OPTIONS) \
		{ #VAR, offsetof(CLASSNAME, VAR), GIZMO_OPTIONS },

#define END_EXPOSE_GIZMOS(CLASSNAME) \
			}; \
			return gizmos; \
		}

	// End-of-class helper: keep as before
#define END_EXPOSE_TO_EDITOR(CLASSNAME) \
		const std::vector<EditorFieldInfo>& GetEditorFields() const override { return CLASSNAME::StaticEditorFields(); } \
		const std::vector<EditorGizmoInfo>& GetEditorGizmos() const override { return CLASSNAME::StaticEditorGizmos(); } \
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

	 // Handy macro that hides the FieldOptions completely and expands to EXPOSE_FIELD for a single-line usage
#define EXPOSE_ENUM_FIELD(Class, Var, UIName, EnumType, ToStringFunc, FIELD_OPTIONS) \
		EXPOSE_FIELD(Class, Var, UIName, FIELD_OPTIONS)

	 // Convenience auto macro using EnumTypeToString naming convention
#define EXPOSE_ENUM_FIELD_AUTO(Class, Var, UIName, EnumType, FIELD_OPTIONS) \
		EXPOSE_ENUM_FIELD(Class, Var, UIName, EnumType, EnumType##ToString, FIELD_OPTIONS)
}

#endif