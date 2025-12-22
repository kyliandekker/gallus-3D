#include "./EditorExpose.h"

#include "graphics/dx12/Transform.h"

#include "core/Engine.h"

// resources
#include "resources/SrcData.h"
#include "resources/EngineResource.h"

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

	void DeserializeEditorExposable(IExposableToEditor* a_pObject, const resources::SrcData& a_SrcData)
	{
		for (const EditorFieldInfo& field : a_pObject->GetEditorFields())
		{
			EditorFieldWidgetType type = field.m_Options.type;

			// These fields are unserializable. They are invalid or purely visual.
			if (type == EditorFieldWidgetType::EditorFieldWidgetType_None || type == EditorFieldWidgetType::EditorFieldWidgetType_TexturePreview || type == EditorFieldWidgetType::EditorFieldWidgetType_Button)
			{
				continue;
			}

			if (field.m_Options.internal)
			{
				continue;
			}
			
			std::string propertyName = TypeNameToPropertyName(field.m_sUIName);

			void* ptr = reinterpret_cast<char*>(a_pObject) + field.m_iOffset;
			
			switch (type)
			{
				case EditorFieldWidgetType::EditorFieldWidgetType_Float:
				{
					a_SrcData.GetFloat(propertyName, *(float*)ptr);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Int8:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int8_t* pValue = reinterpret_cast<int8_t*>(ptr);
					*pValue = value;
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Int16:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int16_t* pValue = reinterpret_cast<int16_t*>(ptr);
					*pValue = value;
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Int32:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int32_t* pValue = reinterpret_cast<int32_t*>(ptr);
					*pValue = value;
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Int64:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int64_t* pValue = reinterpret_cast<int64_t*>(ptr);
					*pValue = value;
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Enum:
				{
					int enumValue = 0;
					a_SrcData.GetInt(propertyName, enumValue);

					int32_t* pEnum = reinterpret_cast<int32_t*>(ptr);
					*pEnum = static_cast<int32_t>(enumValue);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Bool:
				case EditorFieldWidgetType::EditorFieldWidgetType_Switch:
				{
					a_SrcData.GetBool(propertyName, *(bool*)ptr);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Vector2:
				{
					a_SrcData.GetVector2(propertyName, *(DirectX::XMFLOAT2*)ptr);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Vector3:
				{
					a_SrcData.GetVector3(propertyName, *(DirectX::XMFLOAT3*)ptr);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Quaternion:
				{
					DirectX::XMFLOAT3 eulerAngles = VEC_ZERO;
					a_SrcData.GetVector3(propertyName, eulerAngles);
					DirectX::XMVECTOR quaternion = graphics::dx12::Transform::EulerToQuaternion(eulerAngles);
					memcpy(ptr, &quaternion, sizeof(quaternion));
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_EngineResource:
				{
					std::string assetName = "";
					if (!a_SrcData.GetString(propertyName, assetName))
					{
						continue;
					}

					resources::AssetType assetType = field.m_Options.assetType;
					switch (assetType)
					{
						case resources::AssetType::Sprite:
						{
							std::weak_ptr<graphics::dx12::Texture>* pReal =
								reinterpret_cast<std::weak_ptr<graphics::dx12::Texture>*>(ptr);

							auto cCommandQueue =
								core::ENGINE->GetDX12()
								.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

							(*pReal) = core::ENGINE->GetResourceAtlas()
								.LoadTexture(assetName, cCommandQueue);
							break;
						}
						case resources::AssetType::Sound:
						{
							break;
						}
						case resources::AssetType::Song:
						{
							break;
						}
						case resources::AssetType::VO:
						{
							break;
						}
						case resources::AssetType::Animation:
						{
							std::weak_ptr<animation::Animation>* pReal =
								reinterpret_cast<std::weak_ptr<animation::Animation>*>(ptr);

							(*pReal) = core::ENGINE->GetResourceAtlas()
								.LoadAnimation(assetName);
							break;
						}
						case resources::AssetType::PixelShader:
						{
							std::weak_ptr<graphics::dx12::PixelShader>* pReal =
								reinterpret_cast<std::weak_ptr<graphics::dx12::PixelShader>*>(ptr);

							(*pReal) = core::ENGINE->GetResourceAtlas()
								.LoadPixelShader(assetName);
							break;
						}
						case resources::AssetType::VertexShader:
						{
							std::weak_ptr<graphics::dx12::VertexShader>* pReal =
								reinterpret_cast<std::weak_ptr<graphics::dx12::VertexShader>*>(ptr);

							(*pReal) = core::ENGINE->GetResourceAtlas()
								.LoadVertexShader(assetName);
							break;
						}
						case resources::AssetType::Prefab:
						{
							std::weak_ptr<gameplay::Prefab>* pReal =
								reinterpret_cast<std::weak_ptr<gameplay::Prefab>*>(ptr);

							(*pReal) = core::ENGINE->GetResourceAtlas()
								.LoadPrefab(assetName);
							break;
						}
						case resources::AssetType::Mesh:
						{
							std::weak_ptr<graphics::dx12::Mesh>* pReal =
								reinterpret_cast<std::weak_ptr<graphics::dx12::Mesh>*>(ptr);

							auto cCommandQueue =
								core::ENGINE->GetDX12()
								.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

							(*pReal) = core::ENGINE->GetResourceAtlas()
								.LoadMesh(assetName, cCommandQueue);
							break;
						}
						case resources::AssetType::Material:
						{
							break;
						}
						case resources::AssetType::ShaderBind:
						case resources::AssetType::Scene:
						case resources::AssetType::None:
						case resources::AssetType::Folder:
						default:
						{
							break;
						}
					}
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Object:
				{
					IExposableToEditor* editorObject = dynamic_cast<IExposableToEditor*>(reinterpret_cast<IExposableToEditor*>(ptr));
					
					resources::SrcData objectSrcData;
					objectSrcData.SetObject();

					std::string objectPropertyName = TypeNameToPropertyName(editorObject->GetTypeName());
					a_SrcData.GetSrcObject(objectPropertyName, objectSrcData);

					DeserializeEditorExposable(editorObject, objectSrcData);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_ObjectPtr:
				{
					IExposableToEditor** ppEditorObject = reinterpret_cast<IExposableToEditor**>(ptr);
					IExposableToEditor* pEditorObject = (ppEditorObject ? *ppEditorObject : nullptr);
					
					resources::SrcData objectSrcData;
					objectSrcData.SetObject();

					std::string objectPropertyName = TypeNameToPropertyName(pEditorObject->GetTypeName());
					a_SrcData.GetSrcObject(objectPropertyName, objectSrcData);

					DeserializeEditorExposable(pEditorObject, objectSrcData);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Array:
				{
					const ArrayAccessors* pAccessors = field.m_Options.m_pArrayAccessors;
					if (pAccessors == nullptr)
					{
						break;
					}

					resources::SrcData arraySrcData;
					if (!a_SrcData.GetSrcArray(propertyName, arraySrcData))
					{
						break;
					}

					size_t count = 0;
					if (!arraySrcData.GetArraySize(count))
					{
						break;
					}

					void* pArray = ptr;
					pAccessors->Resize(pArray, count);

					for (size_t i = 0; i < count; ++i)
					{
						resources::SrcData elementSrcData;
						if (!arraySrcData.GetSrcArrayElement(i, elementSrcData))
						{
							continue;
						}

						void* pElement = pAccessors->GetElement(pArray, i);

						IExposableToEditor* pEditorElement =
							reinterpret_cast<IExposableToEditor*>(pElement);

						DeserializeEditorExposable(pEditorElement, elementSrcData);
					}

					break;
				}
			}
		}
	}

	void SerializeEditorExposable(const IExposableToEditor* a_pObject, resources::SrcData& a_SrcData)
	{
		for (const EditorFieldInfo& field : a_pObject->GetEditorFields())
		{
			EditorFieldWidgetType type = field.m_Options.type;

			// Internal fields should not be serialized.
			if (field.m_Options.internal)
			{
				continue;
			}

			// These fields are unserializable. They are invalid or purely visual.
			if (type == EditorFieldWidgetType::EditorFieldWidgetType_None || type == EditorFieldWidgetType::EditorFieldWidgetType_TexturePreview || type == EditorFieldWidgetType::EditorFieldWidgetType_Button)
			{
				continue;
			}

			std::string propertyName = TypeNameToPropertyName(field.m_sUIName);

			const void* ptr = reinterpret_cast<const char*>(a_pObject) + field.m_iOffset;

			switch (type)
			{
				case EditorFieldWidgetType::EditorFieldWidgetType_Float:
				{
					const float* value = reinterpret_cast<const float*>(ptr);
					a_SrcData.SetFloat(propertyName, *value);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Int8:
				{
					const int8_t* value = reinterpret_cast<const int8_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Int16:
				{
					const int16_t* value = reinterpret_cast<const int16_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Int32:
				{
					const int32_t* value = reinterpret_cast<const int32_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Int64:
				{
					const int64_t* value = reinterpret_cast<const int64_t*>(ptr);
					int64_t temp = *value;
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Enum:
				{
					const int32_t* value = reinterpret_cast<const int32_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Bool:
				case EditorFieldWidgetType::EditorFieldWidgetType_Switch:
				{
					const bool* value = reinterpret_cast<const bool*>(ptr);
					a_SrcData.SetBool(propertyName, *value);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Vector2:
				{
					const DirectX::XMFLOAT2* value = reinterpret_cast<const DirectX::XMFLOAT2*>(ptr);
					a_SrcData.SetVector2(propertyName, *value);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Vector3:
				{
					const DirectX::XMFLOAT3* value = reinterpret_cast<const DirectX::XMFLOAT3*>(ptr);
					a_SrcData.SetVector3(propertyName, *value);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Quaternion:
				{
					const DirectX::XMVECTOR* value = reinterpret_cast<const DirectX::XMVECTOR*>(ptr);
					const DirectX::XMFLOAT3 eulerAngles = graphics::dx12::Transform::QuaternionToEuler(*value);
					a_SrcData.SetVector3(propertyName, eulerAngles);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_EngineResource:
				{
					const std::weak_ptr<const gallus::resources::EngineResource>* pWeak = reinterpret_cast<const std::weak_ptr<const gallus::resources::EngineResource>*>(ptr);

					if (auto pResource = pWeak->lock())
					{
						switch (field.m_Options.assetType)
						{
						case resources::AssetType::Sprite:
						case resources::AssetType::Mesh:
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
				case EditorFieldWidgetType::EditorFieldWidgetType_Object:
				{
					const IExposableToEditor* editorObject = dynamic_cast<const IExposableToEditor*>(reinterpret_cast<const IExposableToEditor*>(ptr));
					
					resources::SrcData objectSrcData;
					objectSrcData.SetObject();

					SerializeEditorExposable(editorObject, objectSrcData);
					
					std::string objectPropertyName = TypeNameToPropertyName(editorObject->GetTypeName());
					a_SrcData.SetSrcObject(objectPropertyName, objectSrcData);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_ObjectPtr:
				{
					const IExposableToEditor* const* ppEditorObject = reinterpret_cast<const IExposableToEditor* const*>(ptr);
					const IExposableToEditor* pEditorObject = (ppEditorObject ? *ppEditorObject : nullptr);
					
					resources::SrcData objectSrcData;
					objectSrcData.SetObject();

					SerializeEditorExposable(pEditorObject, objectSrcData);
					
					std::string objectPropertyName = TypeNameToPropertyName(pEditorObject->GetTypeName());
					a_SrcData.SetSrcObject(objectPropertyName, objectSrcData);
					break;
				}
				case EditorFieldWidgetType::EditorFieldWidgetType_Array:
				{
					const ArrayAccessors* pAccessors = field.m_Options.m_pArrayAccessors;
					if (pAccessors == nullptr)
					{
						break;
					}

					resources::SrcData arraySrcData;
					arraySrcData.SetArray();

					const void* pArray = ptr;
					size_t count = pAccessors->GetCount(const_cast<void*>(pArray));

					for (size_t i = 0; i < count; ++i)
					{
						const void* pElement = pAccessors->GetElement(const_cast<void*>(pArray), i);

						const IExposableToEditor* pEditorElement =
							reinterpret_cast<const IExposableToEditor*>(pElement);

						resources::SrcData elementSrcData;
						elementSrcData.SetObject();

						SerializeEditorExposable(pEditorElement, elementSrcData);

						arraySrcData.PushArraySrcObject(elementSrcData);
					}

					a_SrcData.SetSrcObject(propertyName, arraySrcData);
					break;
				}
			}
		}
	}
}