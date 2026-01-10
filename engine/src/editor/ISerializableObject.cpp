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
			
			switch (type)
			{
				case FieldSerializationType::FieldSerializationType_Float:
				{
					a_SrcData.GetFloat(propertyName, *(float*)ptr);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int* pValue = reinterpret_cast<int*>(ptr);
					*pValue = value;
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int8:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int8_t* pValue = reinterpret_cast<int8_t*>(ptr);
					*pValue = value;
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int16:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int16_t* pValue = reinterpret_cast<int16_t*>(ptr);
					*pValue = value;
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int32:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int32_t* pValue = reinterpret_cast<int32_t*>(ptr);
					*pValue = value;
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int64:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int64_t* pValue = reinterpret_cast<int64_t*>(ptr);
					*pValue = value;
					break;
				}
				case FieldSerializationType::FieldSerializationType_Enum:
				{
					int enumValue = 0;
					a_SrcData.GetInt(propertyName, enumValue);

					int32_t* pEnum = reinterpret_cast<int32_t*>(ptr);
					*pEnum = static_cast<int32_t>(enumValue);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Bool:
				case FieldSerializationType::FieldSerializationType_Switch:
				{
					a_SrcData.GetBool(propertyName, *(bool*)ptr);
					break;
				}
				case FieldSerializationType::FieldSerializationType_LongSwitch:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int32_t* pValue = reinterpret_cast<int32_t*>(ptr);
					*pValue = value;
					break;
				}
				case FieldSerializationType::FieldSerializationType_Vector2:
				{
					a_SrcData.GetVector2(propertyName, *(DirectX::XMFLOAT2*)ptr);
					break;
				}
				case FieldSerializationType::FieldSerializationType_IVector2:
				{
					a_SrcData.GetIVector2(propertyName, *(DirectX::XMINT2*)ptr);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Vector3:
				{
					a_SrcData.GetVector3(propertyName, *(DirectX::XMFLOAT3*)ptr);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Color:
				{
					a_SrcData.GetColor(propertyName, *(DirectX::XMFLOAT4*)ptr);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Quaternion:
				{
					DirectX::XMFLOAT3 eulerAngles = VEC_ZERO;
					a_SrcData.GetVector3(propertyName, eulerAngles);
					DirectX::XMVECTOR quaternion = graphics::dx12::Transform::EulerToQuaternion(eulerAngles);
					memcpy(ptr, &quaternion, sizeof(quaternion));
					break;
				}
				case FieldSerializationType::FieldSerializationType_EngineResource:
				{
					std::string assetName = "";
					if (!a_SrcData.GetString(propertyName, assetName))
					{
						continue;
					}

					std::weak_ptr<gallus::resources::EngineResource>* weakPtr = reinterpret_cast<std::weak_ptr<gallus::resources::EngineResource>*>(ptr);
					DeserializeResource(weakPtr, field.m_Options.assetType, field.m_Options.assetType, assetName);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Object:
				{
					ISerializableObject* editorObject = dynamic_cast<ISerializableObject*>(reinterpret_cast<ISerializableObject*>(ptr));
					
					resources::SrcData objectSrcData;
					objectSrcData.SetObject();

					std::string objectPropertyName = TypeNameToPropertyName(editorObject->GetTypeName());
					a_SrcData.GetSrcObject(objectPropertyName, objectSrcData);

					DeserializeFields(editorObject, objectSrcData);
					break;
				}
				case FieldSerializationType::FieldSerializationType_ObjectPtr:
				{
					ISerializableObject** ppEditorObject = reinterpret_cast<ISerializableObject**>(ptr);
					ISerializableObject* pEditorObject = (ppEditorObject ? *ppEditorObject : nullptr);
					
					resources::SrcData objectSrcData;
					objectSrcData.SetObject();

					std::string objectPropertyName = TypeNameToPropertyName(pEditorObject->GetTypeName());
					a_SrcData.GetSrcObject(objectPropertyName, objectSrcData);

					DeserializeFields(pEditorObject, objectSrcData);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Array:
				{
					void* arrayPtr = ptr;

					resources::SrcData arrayData;
					if (!a_SrcData.GetSrcArray(propertyName, arrayData))
						break;

					size_t count = 0;
					arrayData.GetArraySize(count);

					field.m_Options.clear(arrayPtr);
					field.m_Options.reserve(arrayPtr, count);

					for (size_t i = 0; i < count; ++i)
					{
						size_t index = field.m_Options.addElement(arrayPtr);

						ISerializableObject* element =
							field.m_Options.getElement(arrayPtr, index);

						resources::SrcData elementData;
						elementData.SetObject();
						arrayData.GetSrcArrayElement(i, elementData);

						DeserializeFields(element, elementData);
					}

					break;
				}
				case FieldSerializationType::FieldSerializationType_String:
				{
					std::string value = "";
					if (!a_SrcData.GetString(propertyName, value))
					{
						continue;
					}

					std::string* pString = reinterpret_cast<std::string*>(ptr);
					*pString = value;
					break;
				}
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

			switch (type)
			{
				case FieldSerializationType::FieldSerializationType_Float:
				{
					const float* value = reinterpret_cast<const float*>(ptr);
					a_SrcData.SetFloat(propertyName, *value);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int:
				{
					const int* value = reinterpret_cast<const int*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int8:
				{
					const int8_t* value = reinterpret_cast<const int8_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int16:
				{
					const int16_t* value = reinterpret_cast<const int16_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int32:
				{
					const int32_t* value = reinterpret_cast<const int32_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int64:
				{
					const int64_t* value = reinterpret_cast<const int64_t*>(ptr);
					int64_t temp = *value;
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Enum:
				{
					const int32_t* value = reinterpret_cast<const int32_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Bool:
				case FieldSerializationType::FieldSerializationType_Switch:
				{
					const bool* value = reinterpret_cast<const bool*>(ptr);
					a_SrcData.SetBool(propertyName, *value);
					break;
				}
				case FieldSerializationType::FieldSerializationType_LongSwitch:
				{
					const int32_t* value = reinterpret_cast<const int32_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Vector2:
				{
					const DirectX::XMFLOAT2* value = reinterpret_cast<const DirectX::XMFLOAT2*>(ptr);
					a_SrcData.SetVector2(propertyName, *value);
					break;
				}
				case FieldSerializationType::FieldSerializationType_IVector2:
				{
					const DirectX::XMINT2* value = reinterpret_cast<const DirectX::XMINT2*>(ptr);
					a_SrcData.SetIVector2(propertyName, *value);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Vector3:
				{
					const DirectX::XMFLOAT3* value = reinterpret_cast<const DirectX::XMFLOAT3*>(ptr);
					a_SrcData.SetVector3(propertyName, *value);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Color:
				{
					const DirectX::XMFLOAT4* value = reinterpret_cast<const DirectX::XMFLOAT4*>(ptr);
					a_SrcData.SetColor(propertyName, *value);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Quaternion:
				{
					const DirectX::XMVECTOR* value = reinterpret_cast<const DirectX::XMVECTOR*>(ptr);
					const DirectX::XMFLOAT3 eulerAngles = graphics::dx12::Transform::QuaternionToEuler(*value);
					a_SrcData.SetVector3(propertyName, eulerAngles);
					break;
				}
				case FieldSerializationType::FieldSerializationType_EngineResource:
				{
					const std::weak_ptr<const gallus::resources::EngineResource>* pWeak = reinterpret_cast<const std::weak_ptr<const gallus::resources::EngineResource>*>(ptr);

					if (auto pResource = pWeak->lock())
					{
						switch (field.m_Options.assetType)
						{
							case resources::AssetType::Sprite:
							case resources::AssetType::Mesh:
							case resources::AssetType::Material:
							case resources::AssetType::PixelShader:
							case resources::AssetType::VertexShader:
							case resources::AssetType::Prefab:
							case resources::AssetType::Animation:
							{
								a_SrcData.SetString(propertyName, pResource->GetName());
								break;
							}
						}
					}

					break;
				}
				case FieldSerializationType::FieldSerializationType_Object:
				{
					const ISerializableObject* editorObject = dynamic_cast<const ISerializableObject*>(reinterpret_cast<const ISerializableObject*>(ptr));
					
					resources::SrcData objectSrcData;
					objectSrcData.SetObject();

					SerializeFields(editorObject, objectSrcData);
					
					std::string objectPropertyName = TypeNameToPropertyName(editorObject->GetTypeName());
					a_SrcData.SetSrcObject(objectPropertyName, objectSrcData);
					break;
				}
				case FieldSerializationType::FieldSerializationType_ObjectPtr:
				{
					const ISerializableObject* const* ppEditorObject = reinterpret_cast<const ISerializableObject* const*>(ptr);
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
					void* pArray = const_cast<void*>(ptr); // SrcData needs non-const

					resources::SrcData arraySrcData;
					arraySrcData.SetArray();

					size_t size = field.m_Options.getSize(pArray);

					for (size_t i = 0; i < size; ++i)
					{
						resources::SrcData elementSrcData;
						elementSrcData.SetObject();
						SerializeFields(field.m_Options.getElement(pArray, i), elementSrcData);
						arraySrcData.PushArraySrcObject(elementSrcData);
					}

					a_SrcData.SetSrcObject(propertyName, arraySrcData);
					break;
				}
				case FieldSerializationType::FieldSerializationType_String:
				{
					const std::string* value = reinterpret_cast<const std::string*>(ptr);
					a_SrcData.SetString(propertyName, *value);
					break;
				}
			}
		}
	}
}