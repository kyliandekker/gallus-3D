#include "./ISerializableObject.h"

// core
#include "core/Engine.h"

// resources
#include "resources/SrcData.h"
#include "resources/EngineResource.h"

// graphics
#include "graphics/dx12/Transform.h"
#include "graphics/dx12/Material.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Shader.h"

// animation
#include "animation/Animation.h"

// gameplay
#include "gameplay/Scene.h"
#include "gameplay/Prefab.h"

// utils
#include "utils/string_extensions.h"

namespace gallus
{
	std::string TypeNameToPropertyName(const std::string& a_sName)
	{
		std::string propertyName = string_extensions::LowerCamelCasify(a_sName);
		propertyName.erase(
			std::remove_if(
				propertyName.begin(),
				propertyName.end(),
				[](unsigned char c)
		{
			return std::isspace(c);
		}
			),
			propertyName.end()
		);
		return propertyName;
	}

	void DeserializeResource(std::weak_ptr<gallus::resources::EngineResource>* a_pWeak, resources::AssetType a_FieldAssetType, resources::AssetType a_AssetType, const std::string& a_sFileName)
	{
		if (a_FieldAssetType != a_AssetType)
		{
			return;
		}

		switch (a_AssetType)
		{
			case resources::AssetType::Sprite:
			{
				auto cCommandQueue =
					core::ENGINE->GetDX12()
					.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

				*a_pWeak = core::ENGINE->GetResourceAtlas()
					.LoadTexture(a_sFileName, cCommandQueue);

				break;
			}
			case resources::AssetType::Mesh:
			{
				auto cCommandQueue =
					core::ENGINE->GetDX12()
					.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

				*a_pWeak = core::ENGINE->GetResourceAtlas()
					.LoadMesh(a_sFileName, cCommandQueue);

				break;
			}
			case resources::AssetType::Material:
			{
				*a_pWeak = core::ENGINE->GetResourceAtlas()
					.LoadMaterial(a_sFileName);

				break;
			}
			case resources::AssetType::PixelShader:
			{
				*a_pWeak = core::ENGINE->GetResourceAtlas()
					.LoadPixelShader(a_sFileName);
				break;
			}
			case resources::AssetType::VertexShader:
			{
				*a_pWeak = core::ENGINE->GetResourceAtlas()
					.LoadVertexShader(a_sFileName);
				break;
			}
			case resources::AssetType::Prefab:
			{
				*a_pWeak = core::ENGINE->GetResourceAtlas()
					.LoadPrefab(a_sFileName);
				break;
			}
			case resources::AssetType::Animation:
			{
				*a_pWeak = core::ENGINE->GetResourceAtlas()
					.LoadAnimation(a_sFileName);
				break;
			}
		}
	}

	void DeserializeValue(const FieldSerializationOptions& a_Options, FieldSerializationType a_FieldSerializationType, void* a_pPtr, const resources::SrcData& a_SrcData, const std::string& a_sPropertyName)
	{
		switch (a_FieldSerializationType)
		{
			case FieldSerializationType::FieldSerializationType_Float:
			{
				a_SrcData.GetFloat(a_sPropertyName, *(float*) a_pPtr);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Int8:
			{
				int32_t value = 0;
				a_SrcData.GetInt(a_sPropertyName, value);
				*(int8_t*) a_pPtr = static_cast<int8_t>(value);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Int16:
			{
				int32_t value = 0;
				a_SrcData.GetInt(a_sPropertyName, value);
				*(int16_t*) a_pPtr = static_cast<int16_t>(value);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Int32:
			{
				int32_t value = 0;
				a_SrcData.GetInt(a_sPropertyName, value);
				*(int32_t*) a_pPtr = value;
				break;
			}
			case FieldSerializationType::FieldSerializationType_Enum:
			{
				int32_t value = 0;
				a_SrcData.GetInt(a_sPropertyName, value);
				*(int32_t*) a_pPtr = value;
				break;
			}
			case FieldSerializationType::FieldSerializationType_Bool:
			case FieldSerializationType::FieldSerializationType_Switch:
			{
				a_SrcData.GetBool(a_sPropertyName, *(bool*) a_pPtr);
				break;
			}
			case FieldSerializationType::FieldSerializationType_LongSwitch:
			{
				int32_t value = 0;
				a_SrcData.GetInt(a_sPropertyName, value);
				*(int32_t*) a_pPtr = value;
				break;
			}
			case FieldSerializationType::FieldSerializationType_Vector2:
			{
				Vector2 vec2;
				a_SrcData.GetVector2(a_sPropertyName, vec2);
				*(DirectX::XMFLOAT2*) a_pPtr = vec2;
				break;
			}
			case FieldSerializationType::FieldSerializationType_IVector2:
			{
				IVector2 ivec2;
				a_SrcData.GetIVector2(a_sPropertyName, ivec2);
				*(DirectX::XMINT2*) a_pPtr = ivec2;
				break;
			}
			case FieldSerializationType::FieldSerializationType_Vector3:
			{
				Vector3 vec3;
				a_SrcData.GetVector3(a_sPropertyName, vec3);
				*(DirectX::XMFLOAT3*) a_pPtr = vec3;
				break;
			}
			case FieldSerializationType::FieldSerializationType_Color:
			{
				Color4 col;
				a_SrcData.GetColor(a_sPropertyName, col);
				*(DirectX::XMFLOAT4*) a_pPtr = col;
				break;
			}
			case FieldSerializationType::FieldSerializationType_Quaternion:
			{
				Vector4 vec4;
				a_SrcData.GetVector4(a_sPropertyName, vec4);
				*(DirectX::XMFLOAT4*) a_pPtr = vec4;
				break;
			}
			case FieldSerializationType::FieldSerializationType_EngineResource:
			{
				std::string assetName = "";
				if (!a_SrcData.GetString(a_sPropertyName, assetName))
				{
					return;
				}

				std::weak_ptr<gallus::resources::EngineResource>* weakPtr = reinterpret_cast<std::weak_ptr<gallus::resources::EngineResource>*>(a_pPtr);
				DeserializeResource(weakPtr, a_Options.assetType, a_Options.assetType, assetName);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Object:
			{
				ISerializableObject* editorObject = dynamic_cast<ISerializableObject*>(reinterpret_cast<ISerializableObject*>(a_pPtr));

				resources::SrcData objectSrcData;
				objectSrcData.SetObject();

				if (a_Options.type == FieldSerializationType::FieldSerializationType_Array)
				{
					DeserializeFields(editorObject, a_SrcData);
				}
				else
				{
					std::string objectPropertyName = TypeNameToPropertyName(editorObject->GetTypeName());
					if (!a_SrcData.GetSrcObject(objectPropertyName, objectSrcData))
					{
						return;
					}

					DeserializeFields(editorObject, objectSrcData);
				}

				break;
			}
			case FieldSerializationType::FieldSerializationType_ObjectPtr:
			{
				ISerializableObject** ppEditorObject = reinterpret_cast<ISerializableObject**>(a_pPtr);
				ISerializableObject* pEditorObject = (ppEditorObject ? *ppEditorObject : nullptr);

				resources::SrcData objectSrcData;
				objectSrcData.SetObject();

				if (!pEditorObject)
				{
					return;
				}

				std::string objectPropertyName = TypeNameToPropertyName(pEditorObject->GetTypeName());
				a_SrcData.GetSrcObject(objectPropertyName, objectSrcData);

				DeserializeFields(pEditorObject, objectSrcData);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Array:
			{
				void* pArray = a_pPtr;

				resources::SrcData arraySrcData;
				if (!a_SrcData.GetSrcArray(a_sPropertyName, arraySrcData))
				{
					break;
				}

				size_t count = 0;
				arraySrcData.GetArraySize(count);

				a_Options.clear(pArray);
				a_Options.reserve(pArray, count);

				for (size_t i = 0; i < count; ++i)
				{
					size_t index = a_Options.addElement(pArray);
					void* pElement = a_Options.getElement(pArray, index);

					resources::SrcData elementSrcData;
					elementSrcData.SetObject();
					if (arraySrcData.GetSrcArrayElement(i, elementSrcData))
					{
						DeserializeValue(a_Options, a_Options.arrayType, pElement, elementSrcData, "");
					}
				}

				break;
			}
			case FieldSerializationType::FieldSerializationType_String:
			{
				std::string value;
				if (a_SrcData.GetString(a_sPropertyName, value))
				{
					*(std::string*) a_pPtr = value;
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}

	void DeserializeFields(ISerializableObject* a_pObject, const resources::SrcData& a_SrcData)
	{
		for (const FieldSerializationInfo& field : a_pObject->GetEditorFields())
		{
			FieldSerializationType type = field.m_Options.type;

			// These fields are unserializable. They are invalid or purely visual.
			if (type == FieldSerializationType::FieldSerializationType_None || type == FieldSerializationType::FieldSerializationType_TexturePreview || type == FieldSerializationType::FieldSerializationType_Button)
			{
				continue;
			}

			if (field.m_Options.internal)
			{
				continue;
			}

			if (!field.m_Options.serialize)
			{
				continue;
			}
			
			std::string propertyName = TypeNameToPropertyName(field.m_sUIName);

			void* ptr = reinterpret_cast<char*>(a_pObject) + field.m_iOffset;

			const resources::SrcData* dataToUse = &a_SrcData;

			DeserializeValue(field.m_Options, field.m_Options.type, ptr, a_SrcData, propertyName);
		}
	}

	void SerializeValue(const FieldSerializationOptions& a_Options, FieldSerializationType a_FieldSerializationType, const void* a_pPtr, resources::SrcData& a_SrcData, const std::string& a_sPropertyName)
	{
		switch (a_FieldSerializationType)
		{
			case FieldSerializationType::FieldSerializationType_Float:
			{
				const float* value = reinterpret_cast<const float*>(a_pPtr);
				a_SrcData.SetFloat(a_sPropertyName, *value);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Int8:
			{
				const int8_t* value = reinterpret_cast<const int8_t*>(a_pPtr);
				int64_t temp = static_cast<int64_t>(*value);
				a_SrcData.SetInt(a_sPropertyName, temp);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Int16:
			{
				const int16_t* value = reinterpret_cast<const int16_t*>(a_pPtr);
				int64_t temp = static_cast<int64_t>(*value);
				a_SrcData.SetInt(a_sPropertyName, temp);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Int32:
			{
				const int32_t* value = reinterpret_cast<const int32_t*>(a_pPtr);
				int64_t temp = static_cast<int64_t>(*value);
				a_SrcData.SetInt(a_sPropertyName, temp);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Enum:
			{
				const int32_t* value = reinterpret_cast<const int32_t*>(a_pPtr);
				int64_t temp = static_cast<int64_t>(*value);
				a_SrcData.SetInt(a_sPropertyName, temp);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Bool:
			case FieldSerializationType::FieldSerializationType_Switch:
			{
				const bool* value = reinterpret_cast<const bool*>(a_pPtr);
				a_SrcData.SetBool(a_sPropertyName, *value);
				break;
			}
			case FieldSerializationType::FieldSerializationType_LongSwitch:
			{
				const int32_t* value = reinterpret_cast<const int32_t*>(a_pPtr);
				int64_t temp = static_cast<int64_t>(*value);
				a_SrcData.SetInt(a_sPropertyName, temp);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Vector2:
			{
				const DirectX::XMFLOAT2* value = reinterpret_cast<const DirectX::XMFLOAT2*>(a_pPtr);
				a_SrcData.SetVector2(a_sPropertyName, *value);
				break;
			}
			case FieldSerializationType::FieldSerializationType_IVector2:
			{
				const DirectX::XMINT2* value = reinterpret_cast<const DirectX::XMINT2*>(a_pPtr);
				a_SrcData.SetIVector2(a_sPropertyName, *value);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Vector3:
			{
				const DirectX::XMFLOAT3* value = reinterpret_cast<const DirectX::XMFLOAT3*>(a_pPtr);
				a_SrcData.SetVector3(a_sPropertyName, *value);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Color:
			{
				const DirectX::XMFLOAT4* value = reinterpret_cast<const DirectX::XMFLOAT4*>(a_pPtr);
				a_SrcData.SetColor(a_sPropertyName, { *value });
				break;
			}
			case FieldSerializationType::FieldSerializationType_Quaternion:
			{
				a_SrcData.SetVector4(a_sPropertyName, *(const DirectX::XMFLOAT4*) a_pPtr);
				break;
			}
			case FieldSerializationType::FieldSerializationType_EngineResource:
			{
				const std::weak_ptr<const gallus::resources::EngineResource>* pWeak = reinterpret_cast<const std::weak_ptr<const gallus::resources::EngineResource>*>(a_pPtr);

				if (auto pResource = pWeak->lock())
				{
					switch (a_Options.assetType)
					{
						case resources::AssetType::Sprite:
						case resources::AssetType::Mesh:
						case resources::AssetType::Material:
						case resources::AssetType::PixelShader:
						case resources::AssetType::VertexShader:
						case resources::AssetType::Prefab:
						case resources::AssetType::Animation:
						{
							a_SrcData.SetString(a_sPropertyName, pResource->GetName());
							break;
						}
					}
				}

				break;
			}
			case FieldSerializationType::FieldSerializationType_Object:
			{
				const ISerializableObject* editorObject = dynamic_cast<const ISerializableObject*>(reinterpret_cast<const ISerializableObject*>(a_pPtr));

				resources::SrcData objectSrcData;
				objectSrcData.SetObject();

				SerializeFields(editorObject, objectSrcData);

				std::string objectPropertyName = TypeNameToPropertyName(editorObject->GetTypeName());
				a_SrcData.SetSrcObject(objectPropertyName, objectSrcData);
				break;
			}
			case FieldSerializationType::FieldSerializationType_ObjectPtr:
			{
				const ISerializableObject* const* ppEditorObject = reinterpret_cast<const ISerializableObject* const*>(a_pPtr);
				const ISerializableObject* pEditorObject = (ppEditorObject ? *ppEditorObject : nullptr);

				resources::SrcData objectSrcData;
				objectSrcData.SetObject();

				SerializeFields(pEditorObject, objectSrcData);

				std::string objectPropertyName = TypeNameToPropertyName(pEditorObject->GetTypeName());
				a_SrcData.SetSrcObject(objectPropertyName, objectSrcData);
				break;
			}
			case FieldSerializationType::FieldSerializationType_Array:
			{
				void* pArray = const_cast<void*>(a_pPtr);

				resources::SrcData arrayData;
				arrayData.SetArray();

				size_t count = a_Options.getSize(pArray);

				for (size_t i = 0; i < count; ++i)
				{
					void* pElement = a_Options.getElement(pArray, i);

					resources::SrcData elementData;
					elementData.SetObject();

					SerializeValue(a_Options, a_Options.arrayType, pElement, elementData, "");

					arrayData.PushArraySrcObject(elementData);
				}

				a_SrcData.SetSrcObject(a_sPropertyName, arrayData);
				break;
			}
			case FieldSerializationType::FieldSerializationType_String:
			{
				const std::string* value = reinterpret_cast<const std::string*>(a_pPtr);
				a_SrcData.SetString(a_sPropertyName, *value);
				break;
			}
		}
	}

	void SerializeFields(const ISerializableObject* a_pObject, resources::SrcData& a_SrcData)
	{
		for (const FieldSerializationInfo& field : a_pObject->GetEditorFields())
		{
			FieldSerializationType type = field.m_Options.type;

			// Internal fields should not be serialized.
			if (field.m_Options.internal)
			{
				continue;
			}

			if (!field.m_Options.serialize)
			{
				continue;
			}

			// These fields are unserializable. They are invalid or purely visual.
			if (type == FieldSerializationType::FieldSerializationType_None || type == FieldSerializationType::FieldSerializationType_TexturePreview || type == FieldSerializationType::FieldSerializationType_Button)
			{
				continue;
			}

			std::string propertyName = TypeNameToPropertyName(field.m_sUIName);

			const void* ptr = reinterpret_cast<const char*>(a_pObject) + field.m_iOffset;

			SerializeValue(field.m_Options, field.m_Options.type, ptr, a_SrcData, propertyName);
		}
	}
}