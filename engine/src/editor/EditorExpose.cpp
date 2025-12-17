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
			if (type == EditorFieldWidgetType::None || type == EditorFieldWidgetType::TexturePreview)
			{
				continue;
			}
			
			std::string propertyName = TypeNameToPropertyName(field.m_sUIName);

			void* ptr = reinterpret_cast<char*>(a_pObject) + field.m_iOffset;
			
			switch (type)
			{
				case EditorFieldWidgetType::DragFloat:
				{
					a_SrcData.GetFloat(propertyName, *(float*)ptr);
					break;
				}
				case EditorFieldWidgetType::DragInt8:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int8_t* pValue = reinterpret_cast<int8_t*>(ptr);
					*pValue = value;
					break;
				}
				case EditorFieldWidgetType::DragInt16:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int16_t* pValue = reinterpret_cast<int16_t*>(ptr);
					*pValue = value;
					break;
				}
				case EditorFieldWidgetType::DragInt32:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int32_t* pValue = reinterpret_cast<int32_t*>(ptr);
					*pValue = value;
					break;
				}
				case EditorFieldWidgetType::DragInt64:
				{
					int value = 0;
					a_SrcData.GetInt(propertyName, value);
					int64_t* pValue = reinterpret_cast<int64_t*>(ptr);
					*pValue = value;
					break;
				}
				case EditorFieldWidgetType::EnumDropdown:
				{
					a_SrcData.GetInt(propertyName, *(int*)ptr);
					break;
				}
				case EditorFieldWidgetType::Checkbox:
				case EditorFieldWidgetType::Toggle:
				{
					a_SrcData.GetBool(propertyName, *(bool*)ptr);
					break;
				}
				case EditorFieldWidgetType::Vector2Field:
				{
					a_SrcData.GetVector2(propertyName, *(DirectX::XMFLOAT2*)ptr);
					break;
				}
				case EditorFieldWidgetType::Vector3Field:
				{
					a_SrcData.GetVector3(propertyName, *(DirectX::XMFLOAT3*)ptr);
					break;
				}
				case EditorFieldWidgetType::Quaternion:
				{
					DirectX::XMFLOAT3 eulerAngles = VEC_ZERO;
					a_SrcData.GetVector3(propertyName, eulerAngles);
					DirectX::XMVECTOR quaternion = graphics::dx12::Transform::EulerToQuaternion(eulerAngles);
					memcpy(ptr, &quaternion, sizeof(quaternion));
					break;
				}
				case EditorFieldWidgetType::AssetPickerPtr:
				{
					std::string assetName = "";
					if (!a_SrcData.GetString(propertyName, assetName))
					{
						continue;
					}

					std::weak_ptr<gallus::resources::EngineResource>* pWeak =
						reinterpret_cast<std::weak_ptr<gallus::resources::EngineResource>*>(ptr);

					if (auto pResource = pWeak->lock())
					{
						auto assetType = pResource->GetAssetType();
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
								std::weak_ptr<animation::AnimationTrack>* pReal =
									reinterpret_cast<std::weak_ptr<animation::AnimationTrack>*>(ptr);

								(*pReal) = core::ENGINE->GetResourceAtlas()
									.LoadAnimationTrack(assetName);
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
					}
					break;
				}
				case EditorFieldWidgetType::Object:
				{
					IExposableToEditor* editorObject = dynamic_cast<IExposableToEditor*>(reinterpret_cast<IExposableToEditor*>(ptr));
					
					resources::SrcData objectSrcData;
					objectSrcData.SetObject();

					std::string objectPropertyName = TypeNameToPropertyName(editorObject->GetTypeName());
					a_SrcData.GetSrcObject(objectPropertyName, objectSrcData);

					DeserializeEditorExposable(editorObject, objectSrcData);
					break;
				}
				case EditorFieldWidgetType::ObjectPtr:
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
			if (type == EditorFieldWidgetType::None || type == EditorFieldWidgetType::TexturePreview)
			{
				continue;
			}

			std::string propertyName = TypeNameToPropertyName(field.m_sUIName);

			const void* ptr = reinterpret_cast<const char*>(a_pObject) + field.m_iOffset;

			switch (type)
			{
				case EditorFieldWidgetType::DragFloat:
				{
					const float* value = reinterpret_cast<const float*>(ptr);
					a_SrcData.SetFloat(propertyName, *value);
					break;
				}
				case EditorFieldWidgetType::DragInt8:
				{
					const int8_t* value = reinterpret_cast<const int8_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case EditorFieldWidgetType::DragInt16:
				{
					const int16_t* value = reinterpret_cast<const int16_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case EditorFieldWidgetType::DragInt32:
				{
					const int32_t* value = reinterpret_cast<const int32_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case EditorFieldWidgetType::DragInt64:
				{
					const int64_t* value = reinterpret_cast<const int64_t*>(ptr);
					int64_t temp = *value;
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case EditorFieldWidgetType::EnumDropdown:
				{
					const int32_t* value = reinterpret_cast<const int32_t*>(ptr);
					int64_t temp = static_cast<int64_t>(*value);
					a_SrcData.SetInt(propertyName, temp);
					break;
				}
				case EditorFieldWidgetType::Checkbox:
				case EditorFieldWidgetType::Toggle:
				{
					const bool* value = reinterpret_cast<const bool*>(ptr);
					a_SrcData.SetBool(propertyName, *value);
					break;
				}
				case EditorFieldWidgetType::Vector2Field:
				{
					const DirectX::XMFLOAT2* value = reinterpret_cast<const DirectX::XMFLOAT2*>(ptr);
					a_SrcData.SetVector2(propertyName, *value);
					break;
				}
				case EditorFieldWidgetType::Vector3Field:
				{
					const DirectX::XMFLOAT3* value = reinterpret_cast<const DirectX::XMFLOAT3*>(ptr);
					a_SrcData.SetVector3(propertyName, *value);
					break;
				}
				case EditorFieldWidgetType::Quaternion:
				{
					const DirectX::XMVECTOR* value = reinterpret_cast<const DirectX::XMVECTOR*>(ptr);
					const DirectX::XMFLOAT3 eulerAngles = graphics::dx12::Transform::QuaternionToEuler(*value);
					a_SrcData.SetVector3(propertyName, eulerAngles);
					break;
				}
				case EditorFieldWidgetType::AssetPickerPtr:
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
						{
							a_SrcData.SetString(propertyName, pResource->GetName());
							break;
						}
						}
					}

					break;
				}
				case EditorFieldWidgetType::Object:
				{
					const IExposableToEditor* editorObject = dynamic_cast<const IExposableToEditor*>(reinterpret_cast<const IExposableToEditor*>(ptr));
					
					resources::SrcData objectSrcData;
					objectSrcData.SetObject();

					SerializeEditorExposable(editorObject, objectSrcData);
					
					std::string objectPropertyName = TypeNameToPropertyName(editorObject->GetTypeName());
					a_SrcData.SetSrcObject(objectPropertyName, objectSrcData);
					break;
				}
				case EditorFieldWidgetType::ObjectPtr:
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
			}
		}
	}
}