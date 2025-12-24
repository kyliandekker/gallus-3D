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

#define BEGIN_EXPOSE_FIELDS(CLASSNAME) \
		public: \
			static const std::vector<EditorFieldInfo>& StaticEditorFields() \
			{ \
				static std::vector<EditorFieldInfo> fields = []() { \
					std::vector<EditorFieldInfo> f;

#define BEGIN_EXPOSE_FIELDS_PARENT(CLASSNAME, PARENT) \
		public: \
			static const std::vector<EditorFieldInfo>& StaticEditorFields() \
			{ \
				static std::vector<EditorFieldInfo> fields = []() { \
					std::vector<EditorFieldInfo> f; \
					const auto& parentFields = PARENT::StaticEditorFields(); \
					f.insert(f.end(), parentFields.begin(), parentFields.end());

#define EXPOSE_FIELD(CLASSNAME, VAR, UINAME, DESCRIPTION, FIELD_OPTIONS) \
					f.push_back({ #VAR, offsetof(CLASSNAME, VAR), UINAME, DESCRIPTION, FIELD_OPTIONS });

#define END_EXPOSE_FIELDS(CLASSNAME) \
					return f; \
				}(); \
				return fields; \
			}

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

#define END_EXPOSE_TO_EDITOR(CLASSNAME) \
		const std::vector<EditorFieldInfo>& GetEditorFields() const override { return CLASSNAME::StaticEditorFields(); } \
		const std::vector<EditorGizmoInfo>& GetEditorGizmos() const override { return CLASSNAME::StaticEditorGizmos(); } \
		const char* GetTypeName() const override { return #CLASSNAME; }

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