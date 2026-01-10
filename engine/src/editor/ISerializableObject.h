#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <type_traits>
#include <memory>

#include "resources/AssetType.h"

namespace gallus
{
	namespace resources
	{
		class EngineResource;
		class SrcData;
	}

	enum class FieldSerializationType
	{
		FieldSerializationType_None,
		FieldSerializationType_Float,
		FieldSerializationType_Int,
		FieldSerializationType_Int8,
		FieldSerializationType_Int16,
		FieldSerializationType_Int32,
		FieldSerializationType_Int64,
		FieldSerializationType_Enum,
		FieldSerializationType_Bool,
		FieldSerializationType_LongSwitch,
		FieldSerializationType_Switch,
		FieldSerializationType_Vector2,
		FieldSerializationType_IVector2,
		FieldSerializationType_Vector3,
		FieldSerializationType_Color,
		FieldSerializationType_Quaternion,
		FieldSerializationType_EngineResource,
		FieldSerializationType_Object,
		FieldSerializationType_ObjectPtr,
		FieldSerializationType_Array,
		FieldSerializationType_TexturePreview,
		FieldSerializationType_Button,
		FieldSerializationType_String
	};

	enum class EditorGizmoType
	{
		EditorGizmoType_None,
		EditorGizmoType_Transform,
		EditorGizmoType_Direction
	};

	class ISerializableObject;

	struct FieldSerializationOptions
	{
		FieldSerializationType type = FieldSerializationType::FieldSerializationType_None;
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

		std::function<size_t(void* arrayPtr)> getSize = nullptr;
		std::function<ISerializableObject* (void* arrayPtr, size_t index)> getElement = nullptr;
		std::function<size_t(void* arrayPtr)> addElement = nullptr;
		std::function<void(void* arrayPtr, size_t index)> removeElement = nullptr;
		std::function<void(void* arrayPtr, size_t amount)> reserve = nullptr;
		std::function<void(void* arrayPtr)> clear = nullptr;
		FieldSerializationType arrayType;
	};

	struct GizmoOptions
	{
		EditorGizmoType type = EditorGizmoType::EditorGizmoType_None;
	};

	struct FieldSerializationInfo
	{
		const char* m_sName;
		size_t m_iOffset;
		const char* m_sUIName;
		std::string m_sDescription;
		FieldSerializationOptions m_Options;
	};

	struct EditorGizmoInfo
	{
		const char* m_sName;
		size_t m_iOffset;
		GizmoOptions m_Options;
	};

	class ISerializableObject
	{
	public:
		virtual const std::vector<FieldSerializationInfo>& GetEditorFields() const = 0;
		virtual const std::vector<EditorGizmoInfo>& GetEditorGizmos() const = 0;
		virtual const char* GetTypeName() const = 0;
		virtual ~ISerializableObject() = default;
	};

	template<typename T>
	inline const std::vector<FieldSerializationInfo>* GetEditorFieldsFor()
	{
		static_assert(std::is_base_of<ISerializableObject, T>::value);
		return &T::StaticEditorFields();
	}

#define BEGIN_SERIALIZE(CLASSNAME) \
		public: \
			using _SerializedClass = CLASSNAME; \
			using _FieldClass = CLASSNAME; \
			using _GizmoClass = CLASSNAME; \
			static std::pair<std::vector<FieldSerializationInfo>, std::vector<EditorGizmoInfo>> _InitializeSerializedFields() { \
				std::vector<FieldSerializationInfo> f; \
				std::vector<EditorGizmoInfo> g;

#define BEGIN_SERIALIZE_PARENT(CLASSNAME, PARENT) \
		public: \
			using _SerializedClass = CLASSNAME; \
			using _FieldClass = CLASSNAME; \
			using _GizmoClass = CLASSNAME; \
			static std::pair<std::vector<FieldSerializationInfo>, std::vector<EditorGizmoInfo>> _InitializeSerializedFields() { \
				std::vector<FieldSerializationInfo> f; \
				std::vector<EditorGizmoInfo> g; \
				const auto& parentFields = PARENT::StaticEditorFields(); \
				f.insert(f.end(), parentFields.begin(), parentFields.end());

#define SERIALIZE_FIELD(VAR, UINAME, DESCRIPTION, ...) \
				f.push_back({ #VAR, offsetof(_FieldClass, VAR), UINAME, DESCRIPTION, FieldSerializationOptions{ __VA_ARGS__ } });

#define SERIALIZE_FIELD_SIMPLE(VAR, UINAME, DESCRIPTION, FIELD_TYPE) \
				f.push_back({ #VAR, offsetof(_FieldClass, VAR), UINAME, DESCRIPTION, FieldSerializationOptions{ .type = FIELD_TYPE } });

#define SERIALIZE_FIELD_OPTIONS(VAR, UINAME, DESCRIPTION, OPTIONS) \
			f.push_back({ #VAR, offsetof(_FieldClass, VAR), UINAME, DESCRIPTION, OPTIONS });

#define EXPOSE_GIZMO(VAR, GIZMO_OPTIONS) \
				g.push_back({ #VAR, offsetof(_GizmoClass, VAR), GIZMO_OPTIONS });

#define END_SERIALIZE(CLASSNAME) \
				return { f, g }; \
			} \
			static const std::vector<FieldSerializationInfo>& StaticEditorFields() { \
				static auto [fields, gizmos] = _InitializeSerializedFields(); \
				return fields; \
			} \
			static const std::vector<EditorGizmoInfo>& StaticEditorGizmos() { \
				static auto [fields, gizmos] = _InitializeSerializedFields(); \
				return gizmos; \
			} \
			const std::vector<FieldSerializationInfo>& GetEditorFields() const override { return _SerializedClass::StaticEditorFields(); } \
			const std::vector<EditorGizmoInfo>& GetEditorGizmos() const override { return _SerializedClass::StaticEditorGizmos(); } \
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
	FieldSerializationOptions MakeArrayFieldSerializationOptions()
	{
		static_assert(std::is_base_of<ISerializableObject, T>::value);

		FieldSerializationOptions opts;
		opts.type = FieldSerializationType::FieldSerializationType_Array;

		opts.getSize = [](void* arrayPtr) -> size_t
			{
				auto& vec = *static_cast<std::vector<T>*>(arrayPtr);
				return vec.size();
			};

		opts.getElement = [](void* arrayPtr, size_t index)
			{
				auto& vec = *static_cast<std::vector<T>*>(arrayPtr);
				return static_cast<ISerializableObject*>(&vec[index]);
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
	void DeserializeResource(std::weak_ptr<gallus::resources::EngineResource>* a_pWeak, resources::AssetType a_FieldAssetType, resources::AssetType a_AssetType, const std::string& a_sFileName);

	void DeserializeFields(ISerializableObject* a_pObject, const resources::SrcData& a_SrcData);

#ifdef _EDITOR
	void SerializeFields(const ISerializableObject* a_pObject, resources::SrcData& a_SrcData);
#endif
		}
