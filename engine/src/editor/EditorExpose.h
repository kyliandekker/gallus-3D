#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <type_traits>

#include "resources/AssetType.h"

namespace gallus
{
	namespace resources
	{
		class SrcData;
	}

	enum class EditorFieldWidgetType
	{
		EditorFieldWidgetType_None,
		EditorFieldWidgetType_Float,
		EditorFieldWidgetType_Int8,
		EditorFieldWidgetType_Int16,
		EditorFieldWidgetType_Int32,
		EditorFieldWidgetType_Int64,
		EditorFieldWidgetType_Enum,
		EditorFieldWidgetType_Bool,
		EditorFieldWidgetType_LongSwitch,
		EditorFieldWidgetType_Switch,
		EditorFieldWidgetType_Vector2,
		EditorFieldWidgetType_Vector3,
		EditorFieldWidgetType_Color,
		EditorFieldWidgetType_Quaternion,
		EditorFieldWidgetType_EngineResource,
		EditorFieldWidgetType_Object,
		EditorFieldWidgetType_ObjectPtr,
		EditorFieldWidgetType_Array,
		EditorFieldWidgetType_TexturePreview,
		EditorFieldWidgetType_Button
	};

	enum class EditorGizmoType
	{
		EditorGizmoType_None,
		EditorGizmoType_Transform
	};

	class IExposableToEditor;

	struct FieldOptions
	{
		EditorFieldWidgetType type = EditorFieldWidgetType::EditorFieldWidgetType_None;
		gallus::resources::AssetType assetType = resources::AssetType::None;
		std::string min = "";
		std::string max = "";
		float step = 1.0f;
		std::function<std::string(int)> enumToStringFunc = nullptr;
		std::function<void(void*)> buttonFunc = nullptr;
		std::function<void(void*)> onChangeFunc = nullptr;
		bool disabled = false;									// If true, field is visible in editor, but cannot interact with it.
		bool internal = false;									// If true, this makes it so that the field is not visible in inspector view, but is visible in system views.
		bool hideInEditor = false;								// If true, makes field invisible in the editor.
		bool serialize = true;									// If false, stops serialization to data.
		size_t relatedIndexFieldOffset = 0;
		size_t indent = 0;

		std::function<size_t(void*)> getSize = nullptr;
		std::function<IExposableToEditor* (void*, size_t)> getElement = nullptr;
		std::function<size_t(void*)> addElement = nullptr;
		std::function<void(void*, size_t)> removeElement = nullptr;
		std::function<void(void*, size_t)> reserve = nullptr;
		std::function<void(void*)> clear = nullptr;
	};

	struct GizmoOptions
	{
		EditorGizmoType type = EditorGizmoType::EditorGizmoType_None;
	};

	struct EditorFieldInfo
	{
		const char* m_sName;
		size_t m_iOffset;
		const char* m_sUIName;
		std::string m_sDescription;
		FieldOptions m_Options;
	};

	struct EditorGizmoInfo
	{
		const char* m_sName;
		size_t m_iOffset;
		GizmoOptions m_Options;
	};

	class IExposableToEditor
	{
	public:
		virtual const std::vector<EditorFieldInfo>& GetEditorFields() const = 0;
		virtual const std::vector<EditorGizmoInfo>& GetEditorGizmos() const = 0;
		virtual const char* GetTypeName() const = 0;
		virtual ~IExposableToEditor() = default;
	};

	template<typename T>
	inline const std::vector<EditorFieldInfo>* GetEditorFieldsFor()
	{
		static_assert(std::is_base_of<IExposableToEditor, T>::value);
		return &T::StaticEditorFields();
	}

// ===== SIMPLIFIED MACRO SYSTEM (v2) =====
// Ultra-clean syntax: just BEGIN_EXPOSABLE, then EXPOSE_FIELD/EXPOSE_GIZMO, then END_EXPOSABLE
// Class name is captured automatically - no need to pass it or repeat it!

// Main macro: Sets up field and gizmo static methods
// Usage: Place once in the public section of your class
#define BEGIN_EXPOSABLE(CLASSNAME) \
		public: \
			using _ExposableClass = CLASSNAME; \
			using _FieldClass = CLASSNAME; \
			using _GizmoClass = CLASSNAME; \
			static const std::vector<EditorFieldInfo>& StaticEditorFields() \
			{ \
				static std::vector<EditorFieldInfo> fields = []() { \
					std::vector<EditorFieldInfo> f;

// Main macro with parent class inheritance
#define BEGIN_EXPOSABLE_PARENT(CLASSNAME, PARENT) \
		public: \
			using _ExposableClass = CLASSNAME; \
			using _FieldClass = CLASSNAME; \
			using _GizmoClass = CLASSNAME; \
			static const std::vector<EditorFieldInfo>& StaticEditorFields() \
			{ \
				static std::vector<EditorFieldInfo> fields = []() { \
					std::vector<EditorFieldInfo> f; \
					const auto& parentFields = PARENT::StaticEditorFields(); \
					f.insert(f.end(), parentFields.begin(), parentFields.end());

// Add a field with full options
#define EXPOSE_FIELD(VAR, UINAME, DESCRIPTION, ...) \
					f.push_back({ #VAR, offsetof(_FieldClass, VAR), UINAME, DESCRIPTION, FieldOptions{ __VA_ARGS__ } });

// Add a field with just type (simple cases)
#define EXPOSE_FIELD_SIMPLE(VAR, UINAME, FIELD_TYPE) \
					f.push_back({ #VAR, offsetof(_FieldClass, VAR), UINAME, "", FieldOptions{ .type = FIELD_TYPE } });

// Add a gizmo (no class name needed - uses _GizmoClass from BEGIN_EXPOSABLE)
// NOTE: `EXPOSE_GIZMO` must be used after `END_EXPOSE_FIELDS()` (or you may call
// `END_EXPOSABLE(CLASSNAME)` immediately after fields if there are no gizmos).
// This keeps macro behavior simple and predictable.
#define EXPOSE_GIZMO(VAR, GIZMO_OPTIONS) \
					g.push_back({ #VAR, offsetof(_GizmoClass, VAR), GIZMO_OPTIONS });

// Close the entire exposable section (generates GetEditorFields, GetEditorGizmos, GetTypeName overrides)
// CLASSNAME is needed here to stringify it for GetTypeName override
#define END_EXPOSABLE(CLASSNAME) \
					return f; \
				}(); \
				return fields; \
			} \
			static const std::vector<EditorGizmoInfo>& StaticEditorGizmos() \
			{ \
				static std::vector<EditorGizmoInfo> gizmos = []() { \
					std::vector<EditorGizmoInfo> g; \
					return g; \
				}(); \
				return gizmos; \
			} \
			const std::vector<EditorFieldInfo>& GetEditorFields() const override { return _ExposableClass::StaticEditorFields(); } \
			const std::vector<EditorGizmoInfo>& GetEditorGizmos() const override { return _ExposableClass::StaticEditorGizmos(); } \
			const char* GetTypeName() const override { return #CLASSNAME; }

// Variant for when you only have gizmos (no END_EXPOSE_FIELDS needed)
/* Removed END_EXPOSABLE_WITH_GIZMOS helper - behavior unified into END_EXPOSABLE(CLASSNAME).
   To declare gizmos, call `END_EXPOSE_FIELDS()` first, then `EXPOSE_GIZMO(...)` calls,
   and finally `END_EXPOSABLE(CLASSNAME)`. This keeps macro control flow explicit and
   avoids preprocessor state complexities. */

// ===== BACKWARD COMPATIBILITY - OLD STYLE MACROS =====
// These old macros still work for gradual migration (not recommended for new code)

#define BEGIN_EXPOSE_FIELDS(CLASSNAME) \
		public: \
			static const std::vector<EditorFieldInfo>& StaticEditorFields() \
			{ \
				static std::vector<EditorFieldInfo> fields = []() { \
					std::vector<EditorFieldInfo> f; \
					using _FieldClass = CLASSNAME;

#define BEGIN_EXPOSE_FIELDS_PARENT(CLASSNAME, PARENT) \
		public: \
			static const std::vector<EditorFieldInfo>& StaticEditorFields() \
			{ \
				static std::vector<EditorFieldInfo> fields = []() { \
					std::vector<EditorFieldInfo> f; \
					using _FieldClass = CLASSNAME; \
					const auto& parentFields = PARENT::StaticEditorFields(); \
					f.insert(f.end(), parentFields.begin(), parentFields.end());

/* Backwards compatibility: provide END_EXPOSE_FIELDS and END_EXPOSE_GIZMOS
	so older code can still use the legacy macros. These close the lambdas
	opened by BEGIN_EXPOSE_FIELDS / BEGIN_EXPOSE_GIZMOS and leave the
	overrides to `END_EXPOSE_TO_EDITOR`. New code should prefer the
	unified BEGIN_EXPOSABLE / END_EXPOSABLE pattern. */

#define END_EXPOSE_FIELDS(CLASSNAME) \
						  return f; \
					 }(); \
					 return fields; \
				}

#define BEGIN_EXPOSE_GIZMOS(CLASSNAME) \
		public: \
			static const std::vector<EditorGizmoInfo>& StaticEditorGizmos() \
			{ \
				static std::vector<EditorGizmoInfo> gizmos = []() { \
					std::vector<EditorGizmoInfo> g; \
					using _GizmoClass = CLASSNAME;

// Close gizmos lambda (legacy macro)
#define END_EXPOSE_GIZMOS(CLASSNAME) \
					return g; \
				}(); \
				return gizmos; \
			}

#define END_EXPOSE_TO_EDITOR(CLASSNAME) \
		const std::vector<EditorFieldInfo>& GetEditorFields() const override { return CLASSNAME::StaticEditorFields(); } \
		const std::vector<EditorGizmoInfo>& GetEditorGizmos() const override { return CLASSNAME::StaticEditorGizmos(); } \
		const char* GetTypeName() const override { return #CLASSNAME; }

#define EXPOSE_FIELD_LEGACY(CLASSNAME, VAR, UINAME, DESCRIPTION, FIELD_OPTIONS) \
					f.push_back({ #VAR, offsetof(CLASSNAME, VAR), UINAME, DESCRIPTION, FIELD_OPTIONS });

#define EXPOSE_GIZMO_LEGACY(CLASSNAME, VAR, GIZMO_OPTIONS) \
			g.push_back({ #VAR, offsetof(CLASSNAME, VAR), GIZMO_OPTIONS });

	template<typename TEnum>
	inline std::function<std::string(int)> MakeEnumToStringFunc(std::string(*toStringFunc)(TEnum))
	{
		return [toStringFunc](int value) -> std::string
			{
				return toStringFunc(static_cast<TEnum>(value));
			};
	}

	template<typename TObject>
	inline std::function<void(void*)> MakeButtonFunc(void (TObject::* a_pFunc)())
	{
		return [a_pFunc](void* a_pObject) -> void
			{
				TObject* pObject = static_cast<TObject*>(a_pObject);
				(pObject->*a_pFunc)();
			};
	}

	template<typename TObject>
	inline std::function<void(void*)> MakeOnChangeFunc(void (TObject::* a_pFunc)())
	{
		return [a_pFunc](void* a_pObject) -> void
			{
				TObject* pObject = static_cast<TObject*>(a_pObject);
				(pObject->*a_pFunc)();
			};
	}

	template<typename T>
	FieldOptions MakeArrayFieldOptions()
	{
		static_assert(std::is_base_of<IExposableToEditor, T>::value);

		FieldOptions opts;
		opts.type = EditorFieldWidgetType::EditorFieldWidgetType_Array;

		opts.getSize = [](void* arrayPtr) -> size_t
			{
				auto& vec = *static_cast<std::vector<T>*>(arrayPtr);
				return vec.size();
			};

		opts.getElement = [](void* arrayPtr, size_t index)
			{
				auto& vec = *static_cast<std::vector<T>*>(arrayPtr);
				return static_cast<IExposableToEditor*>(&vec[index]);
			};

		opts.addElement = [](void* arrayPtr)
			{
				auto& vec = *static_cast<std::vector<T>*>(arrayPtr);
				vec.emplace_back();
				return vec.size() - 1;
			};

		opts.removeElement = [](void* arrayPtr, size_t index)
			{
				auto& vec = *static_cast<std::vector<T>*>(arrayPtr);
				vec.erase(vec.begin() + index);
			};

		opts.reserve = [](void* arrayPtr, size_t size)
			{
				auto& vec = *static_cast<std::vector<T>*>(arrayPtr);
				vec.reserve(size);
			};

		opts.clear = [](void* arrayPtr)
			{
				auto& vec = *static_cast<std::vector<T>*>(arrayPtr);
				vec.clear();
			};

		return opts;
	}

	void DeserializeEditorExposable(IExposableToEditor* a_pObject, const resources::SrcData& a_SrcData);

#ifdef _EDITOR
	void SerializeEditorExposable(const IExposableToEditor* a_pObject, resources::SrcData& a_SrcData);
#endif
		}