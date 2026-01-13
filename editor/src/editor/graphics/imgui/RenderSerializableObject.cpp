#include "RenderSerializableObject.h"

// external
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_toggle.h>
#include <imgui/imgui_helpers.h>
#include <limits>

// utils
#include "utils/string_extensions.h"

// graphics
#include "graphics/dx12/Transform.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Material.h"

// resources
#include "resources/SrcData.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/ISerializableObject.h"
#include "editor/graphics/imgui/EditorWindowsConfig.h"

// gameplay
#include "gameplay/Prefab.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			bool ShowDragFloat(const std::string& a_sId, float* a_pValue, const FieldSerializationInfo& a_Field)
			{
				float min = -FLT_MAX;
				float max = FLT_MAX;
				if (!a_Field.m_Options.min.empty())
				{
					min = std::stof(a_Field.m_Options.min);
				}
				if (!a_Field.m_Options.max.empty())
				{
					max = std::stof(a_Field.m_Options.max);
				}
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				return ImGui::DragFloat(a_sId.c_str(), a_pValue, a_Field.m_Options.step, min, max);
			}

			bool ShowDragInt(const std::string& a_sId, int& a_Value, const FieldSerializationInfo& a_Field)
			{
				int min = std::numeric_limits<int>::min();
				int max = std::numeric_limits<int>::max();

				if (!a_Field.m_Options.min.empty())
				{
					min = std::stoi(a_Field.m_Options.min);
				}
				if (!a_Field.m_Options.max.empty())
				{
					max = std::stoi(a_Field.m_Options.max);
				}

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

				return ImGui::DragInt(a_sId.c_str(), &a_Value, a_Field.m_Options.step, min, max);
			}

			bool ShowVector2(const std::string& a_sId, DirectX::XMFLOAT2& a_vVector, const FieldSerializationInfo& a_Field)
			{
				float val[2] = {
					a_vVector.x,
					a_vVector.y,
				};
				float min = -FLT_MAX;
				float max = FLT_MAX;
				if (!a_Field.m_Options.min.empty())
				{
					min = std::stof(a_Field.m_Options.min);
				}
				if (!a_Field.m_Options.max.empty())
				{
					max = std::stof(a_Field.m_Options.max);
				}
				float availW = ImGui::GetContentRegionAvail().x;
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::VectorEdit2(a_sId.c_str(), val, 0.1f, min, max))
				{
					a_vVector.x = val[0];
					a_vVector.y = val[1];
					return true;
				}
				return false;
			}

			bool ShowVector3(const std::string& a_sId, DirectX::XMFLOAT3& a_vVector, const FieldSerializationInfo& a_Field)
			{
				float val[3] = {
					a_vVector.x,
					a_vVector.y,
					a_vVector.z,
				};
				float min = -FLT_MAX;
				float max = FLT_MAX;
				if (!a_Field.m_Options.min.empty())
				{
					min = std::stof(a_Field.m_Options.min);
				}
				if (!a_Field.m_Options.max.empty())
				{
					max = std::stof(a_Field.m_Options.max);
				}
				float availW = ImGui::GetContentRegionAvail().x;
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::VectorEdit3(a_sId.c_str(), val, a_Field.m_Options.step, min, max))
				{
					a_vVector.x = val[0];
					a_vVector.y = val[1];
					a_vVector.z = val[2];
					return true;
				}
				return false;
			}

			bool ShowQuaternion(const std::string& a_sId, DirectX::XMVECTOR& a_vVector, const FieldSerializationInfo& a_Field)
			{
				DirectX::XMFLOAT3 preRotationDegrees = graphics::dx12::Transform::QuaternionToEuler(a_vVector);

				float val[3] = {
					preRotationDegrees.x,
					preRotationDegrees.y,
					preRotationDegrees.z,
				};

				float min = -FLT_MAX;
				float max = FLT_MAX;
				if (!a_Field.m_Options.min.empty())
				{
					min = std::stof(a_Field.m_Options.min);
				}
				if (!a_Field.m_Options.max.empty())
				{
					max = std::stof(a_Field.m_Options.max);
				}
				float availW = ImGui::GetContentRegionAvail().x;
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::VectorEdit3(a_sId.c_str(), val, 0.1f, min, max))
				{
					DirectX::XMFLOAT3 newRotationDegrees = {
						val[0] - preRotationDegrees.x,
						val[1] - preRotationDegrees.y,
						val[2] - preRotationDegrees.z
					};

					a_vVector = graphics::dx12::Transform::AddRotation(a_vVector, newRotationDegrees);

					return true;
				}
				return false;
			}

			bool ShowObject(ISerializableObject* obj, bool a_bInternal)
			{
				if (!obj)
				{
					ImGui::TextDisabled("<null>");
					return false;
				}

				bool changed = false;
				for (const FieldSerializationInfo& subField : obj->GetEditorFields())
				{
					if (ShowEditorFieldFromObject(obj, subField, a_bInternal))
					{
						changed = true;
					}
				}
				return changed;
			}

			bool ShowAssetPicker(
				const std::string& a_sId,
				gallus::resources::EngineResource* a_pLocked,
				std::weak_ptr<gallus::resources::EngineResource>* a_pWeak,
				ISerializableObject* a_pObject,
				const FieldSerializationInfo& a_Field)
			{
				bool changed = false;

				const std::string name = (a_pLocked != nullptr) ? a_pLocked->GetName() : "<null>";

				char buf[256];
				strncpy_s(buf, sizeof(buf), name.c_str(), sizeof(buf) - 1);
				buf[sizeof(buf) - 1] = '\0';

				ImVec2 buttonSize = ImVec2(
					core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() * 2,
					core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() * 2);

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
					ImVec2(
						core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() / 2,
						core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() / 2));

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(a_sId.c_str(), buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EXPLORER_ITEM"))
					{
						IM_ASSERT(payload->DataSize == sizeof(resources::FileResource*));
						resources::FileResource* dropped = *(resources::FileResource**) payload->Data;
						if (dropped)
						{
							DeserializeResource(a_pWeak, a_Field.m_Options.assetType, dropped->GetAssetType(), dropped->GetPath().filename().generic_string());
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();

				if (ImGui::Button(
					ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, a_sId + BUTTON_ID, "").c_str(),
					buttonSize))
				{
					changed = true;

					FilePickerModal& filePickerModal =
						core::EDITOR_ENGINE->GetDX12().GetImGuiWindow()
						.GetWindowsConfig<EditorWindowsConfig>().GetFilePickerModal();

					filePickerModal.SetData(
						[a_pWeak, &a_Field, a_pObject](int success, const std::string& resourceName)
						{
							if (success == 1)
							{
								DeserializeResource(a_pWeak, a_Field.m_Options.assetType, a_Field.m_Options.assetType, resourceName);
								a_pObject->MarkFieldChanged(&a_Field);
							}
						},
						a_Field.m_Options.assetType);

					filePickerModal.Show();
				}

				ImGui::ShowTooltip(a_Field.m_sDescription);
				ImGui::PopStyleVar();

				if (a_pObject->ConsumeFieldChanged(&a_Field))
				{
					changed = true;
				}

				return changed;
			}

			bool ShowEnumDropdown(const std::string& a_sId, int* a_pValue, const FieldSerializationInfo& a_Field)
			{
				if (!a_Field.m_Options.enumToStringFunc)
				{
					ImGui::TextDisabled("<no enum mapping>");
					return false;
				}

				// Dynamically detect number of enum entries
				std::vector<std::string> names;
				names.reserve(32); // typical enum sizes

				for (int i = 0; i < 512; i++) // upper bound to avoid infinite loops
				{
					std::string name = a_Field.m_Options.enumToStringFunc(i);
					if (name.empty())
					{
						break; // stop when converter returns empty string
					}
					names.push_back(name);
				}

				if (names.empty())
				{
					ImGui::TextDisabled("<no enum values>");
					return false;
				}

				int currentIndex = *a_pValue;
				if (currentIndex < 0 || currentIndex >= static_cast<int>(names.size()))
				{
					currentIndex = 0;
				}

				bool changed = false;
				const char* currentLabel = names[currentIndex].c_str();

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::BeginCombo(a_sId.c_str(), currentLabel))
				{
					for (int i = 0; i < static_cast<int>(names.size()); i++)
					{
						bool isSelected = (i == currentIndex);
						if (ImGui::Selectable(names[i].c_str(), isSelected))
						{
							if (!isSelected)
							{
								*a_pValue = i;
								changed = true;
							}
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::ShowTooltip(a_Field.m_sDescription);

				return changed;
			}

			bool ShowTexturePreview(const std::string& a_sId, graphics::dx12::Texture* a_pTexture, int a_iSpriteIndex, const ImVec2& a_vSize)
			{
				if (!a_pTexture)
				{
					ImGui::TextDisabled("<no texture>");
					return false;
				}

				// Get the sprite UVs from the texture atlas
				const ImVec2 uv0 = ImVec2(a_pTexture->GetSpriteUV(a_iSpriteIndex).uv0.x, a_pTexture->GetSpriteUV(a_iSpriteIndex).uv0.y);  // top-left UV
				const ImVec2 uv1 = ImVec2(a_pTexture->GetSpriteUV(a_iSpriteIndex).uv1.x, a_pTexture->GetSpriteUV(a_iSpriteIndex).uv1.y); // bottom-right UV

				ImGui::Image(a_pTexture->GetGPUHandle().ptr, a_vSize, uv0, uv1);

				return false; // read-only
			}

			bool ShowEditorFieldFromObject(ISerializableObject* a_pObject, const FieldSerializationInfo& a_Field, bool a_bInternal)
			{
				if (a_Field.m_Options.internal != a_bInternal)
				{
					return false;
				}

				void* ptr = reinterpret_cast<char*>(a_pObject) + a_Field.m_iOffset;

				std::function<bool()> func = [] { return false; };

				bool showTable = true;
				std::string fieldId = ImGui::IMGUI_FORMAT_ID("", INPUT_ID, string_extensions::StringToUpper(a_Field.m_sUIName) + "_INSPECTOR");
				switch (a_Field.m_Options.type)
				{
				case FieldSerializationType::FieldSerializationType_Float:
				{
					float* value = reinterpret_cast<float*>(ptr);
					func = [value, &a_Field, &fieldId]
						{
							bool val = ShowDragFloat(fieldId, value, a_Field);
							ImGui::ShowTooltip(a_Field.m_sDescription);
							return val;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int8:
				{
					func = [ptr, &a_Field, &fieldId]
						{
							int8_t* value = reinterpret_cast<int8_t*>(ptr);
							int temp = static_cast<int64_t>(*value);

							bool changed = ShowDragInt(fieldId, temp, a_Field);
							ImGui::ShowTooltip(a_Field.m_sDescription);
							if (changed)
							{
								*value = static_cast<int8_t>(temp);
							}
							return changed;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int16:
				{
					func = [ptr, &a_Field, &fieldId]
						{
							int16_t* value = reinterpret_cast<int16_t*>(ptr);
							int temp = static_cast<int64_t>(*value);

							bool changed = ShowDragInt(fieldId, temp, a_Field);
							ImGui::ShowTooltip(a_Field.m_sDescription);
							if (changed)
							{
								*value = static_cast<int16_t>(temp);
							}
							return changed;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int32:
				{
					func = [ptr, &a_Field, &fieldId]
						{
							int32_t* value = reinterpret_cast<int32_t*>(ptr);
							int temp = static_cast<int64_t>(*value);

							bool changed = ShowDragInt(fieldId, temp, a_Field);
							ImGui::ShowTooltip(a_Field.m_sDescription);
							if (changed)
							{
								*value = static_cast<int32_t>(temp);
							}
							return changed;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_Int64:
				{
					func = [ptr, &a_Field, &fieldId]
						{
							int64_t* value = reinterpret_cast<int64_t*>(ptr);
							int temp = *value;

							bool changed = ShowDragInt(fieldId, temp, a_Field);
							ImGui::ShowTooltip(a_Field.m_sDescription);
							if (changed)
							{
								*value = temp;
							}
							return changed;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_Bool:
				{
					bool* value = reinterpret_cast<bool*>(ptr);
					func = [&a_Field, &fieldId, value]
						{
							bool val = ImGui::Checkbox(fieldId.c_str(), value);
							ImGui::ShowTooltip(a_Field.m_sDescription);
							return val;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_Switch:
				{
					int32_t* value = reinterpret_cast<int32_t*>(ptr);

					bool bValue = *value;
					func = [&a_Field, &fieldId, &bValue, value]
						{
							bool val = ImGui::Toggle(fieldId.c_str(), &bValue);
							*value = bValue;
							ImGui::ShowTooltip(a_Field.m_sDescription);
							return val;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_LongSwitch:
				{
					uint32_t* pValue = reinterpret_cast<uint32_t*>(ptr);

					func = [&a_Field, &fieldId, pValue]()
						{
							bool bValue = (*pValue != 0);

							bool bChanged = ImGui::Toggle(fieldId.c_str(), &bValue);
							ImGui::ShowTooltip(a_Field.m_sDescription);

							if (bChanged)
							{
								*pValue = bValue ? 1u : 0u;
							}

							return bChanged;
						};

					break;
				}
				case FieldSerializationType::FieldSerializationType_Vector2:
				{
					DirectX::XMFLOAT2* value = reinterpret_cast<DirectX::XMFLOAT2*>(ptr);
					func = [&a_Field, &fieldId, value]
						{
							bool val = ShowVector2(fieldId, *value, a_Field);
							ImGui::ShowTooltip(a_Field.m_sDescription);
							return val;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_Vector3:
				{
					DirectX::XMFLOAT3* value = reinterpret_cast<DirectX::XMFLOAT3*>(ptr);
					func = [&a_Field, &fieldId, value]
						{
							bool val = ShowVector3(fieldId, *value, a_Field);
							ImGui::ShowTooltip(a_Field.m_sDescription);
							return val;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_Color:
				{
					DirectX::XMFLOAT3* value = reinterpret_cast<DirectX::XMFLOAT3*>(ptr);
					func = [&a_Field, &fieldId, value]
						{
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
							bool val = ImGui::ColorEdit4(fieldId.c_str(),
								reinterpret_cast<float*>(value)
							);
							ImGui::ShowTooltip(a_Field.m_sDescription);
							return val;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_Quaternion:
				{
					DirectX::XMVECTOR* value = reinterpret_cast<DirectX::XMVECTOR*>(ptr);

					func = [&a_Field, &fieldId, value]
						{
							bool val = ShowQuaternion(fieldId, *value, a_Field);
							ImGui::ShowTooltip(a_Field.m_sDescription);
							return val;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_EngineResource:
				{
					std::weak_ptr<gallus::resources::EngineResource>* pWeak =
						reinterpret_cast<std::weak_ptr<gallus::resources::EngineResource>*>(ptr);

					func = [&a_Field, &fieldId, pWeak, a_pObject]()
						{
							std::shared_ptr<gallus::resources::EngineResource> locked;
							if (pWeak)
							{
								locked = pWeak->lock();
							}

							gallus::resources::EngineResource* pLocked =
								(locked.get()) ? locked.get() : nullptr;

							return ShowAssetPicker(fieldId, pLocked, pWeak, a_pObject, a_Field);
						};

					break;
				}
				case FieldSerializationType::FieldSerializationType_Object:
				{
					showTable = true;
					ISerializableObject* editorObject = dynamic_cast<ISerializableObject*>(reinterpret_cast<ISerializableObject*>(ptr));

					return ShowObject(editorObject, a_bInternal);
					break;
				}
				case FieldSerializationType::FieldSerializationType_ObjectPtr:
				{
					showTable = true;

					ISerializableObject** ppEditorObject = reinterpret_cast<ISerializableObject**>(ptr);
					ISerializableObject* pEditorObject = (ppEditorObject ? *ppEditorObject : nullptr);

					return ShowObject(pEditorObject, a_bInternal);
					break;
				}
				case FieldSerializationType::FieldSerializationType_Enum:
				{
					func = [ptr, &a_Field, &fieldId]
						{
							int* enumValue = reinterpret_cast<int*>(ptr);
							int temp = *enumValue;

							bool changed = ShowEnumDropdown(fieldId, &temp, a_Field);
							if (changed)
							{
								*enumValue = temp;
							}
							return changed;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_TexturePreview:
				{
					func = [ptr, &a_Field, a_pObject]()
						{
							// texture pointer
							graphics::dx12::Texture* tex = *reinterpret_cast<graphics::dx12::Texture**>(ptr);

							// sprite index is int8_t
							int8_t* spriteIndexPtr = reinterpret_cast<int8_t*>(reinterpret_cast<char*>(a_pObject) + a_Field.m_Options.relatedIndexFieldOffset);
							int spriteIndex = spriteIndexPtr ? static_cast<int>(*spriteIndexPtr) : 0;

							ShowTexturePreview(a_Field.m_sUIName, tex, spriteIndex);
							return false;
						};
					break;
				}
				case FieldSerializationType::FieldSerializationType_Button:
				{
					func = [ptr, &a_Field, a_pObject]()
						{
							if (ImGui::TextButton(a_Field.m_sUIName, a_Field.m_sDescription))
							{
								if (a_Field.m_Options.buttonFunc)
								{
									a_Field.m_Options.buttonFunc(a_pObject);
								}
							}
							return false;
						};
					break;
				}
				default:
				{
					break;
				}
				}

				if (showTable)
				{
					return ImGui::KeyValue([&a_Field]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetBoldFont(), a_Field.m_sUIName);
							ImGui::ShowTooltip(a_Field.m_sDescription);
						}, [func, &a_Field]() {
							bool wasDisabled = a_Field.m_Options.disabled;
							if (wasDisabled)
							{

								ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
								ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
							}

							bool success = func();

							if (wasDisabled)
							{
								ImGui::PopItemFlag();
								ImGui::PopStyleVar();
							}

							return success;
							});
				}

				return false;
			}

			bool RenderObjectFields(ISerializableObject* a_pObject, bool a_bInternal)
			{
				const auto& fields = a_pObject->GetEditorFields();

				std::string id = ImGui::IMGUI_FORMAT_ID("", TABLE_ID, string_extensions::StringToUpper(std::string(a_pObject->GetTypeName())) + "_INSPECTOR");
				bool tableActive = ImGui::StartInspectorKeyVal(id, core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFramePadding());

				bool changed = false;
				if (tableActive)
				{
					for (const FieldSerializationInfo& field : fields)
					{
						if (!a_bInternal && field.m_Options.internal)
						{
							continue;
						}

						for (size_t i = 0; i < field.m_Options.indent; i++)
						{
							ImGui::Indent();
						}
						if (ShowEditorFieldFromObject(a_pObject, field, field.m_Options.internal))
						{
							changed = true;
							if (field.m_Options.onChangeFunc)
							{
								field.m_Options.onChangeFunc(a_pObject);
							}
						}
						for (size_t i = 0; i < field.m_Options.indent; i++)
						{
							ImGui::Unindent();
						}
					}
					ImGui::EndInspectorKeyVal(ImVec2());
				}

				return changed;
			}

			bool ShowTransformGizmo(const ImRect& a_SceneViewRect, graphics::dx12::Transform& a_Transform)
			{
				DirectX::XMMATRIX pivotOffset = DirectX::XMMatrixTranslation(a_Transform.GetPivot().x, a_Transform.GetPivot().y, 0.0f);
				DirectX::XMMATRIX objectMat = a_Transform.GetWorldMatrix();
				objectMat = objectMat * pivotOffset;;

				graphics::dx12::Camera& cam = core::ENGINE->GetDX12().GetActiveCamera();

				DirectX::XMMATRIX viewMat = cam.GetViewMatrix(a_Transform.GetCameraType());
				const DirectX::XMMATRIX& projMat = cam.GetProjectionMatrix(a_Transform.GetCameraType());

				// Convert DirectX matrices to float[16] format for ImGuizmo
				float objectFloat[16];
				DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(objectFloat), objectMat);
				float viewFloat[16];
				DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(viewFloat), viewMat);
				float projFloat[16];
				DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(projFloat), projMat);

				bool useSnap = ImGui::IsKeyDown(ImGuiKey_LeftShift);
				float snap = useSnap ? 1.0f : 0.0f;

				if (ImGuizmo::Manipulate(
					viewFloat,
					projFloat,
					(ImGuizmo::OPERATION)core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation(),
					ImGuizmo::WORLD,
					objectFloat, 0, &snap))
				{
					DirectX::XMMATRIX result = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(objectFloat));
					result = result * DirectX::XMMatrixInverse(nullptr, pivotOffset);

					a_Transform.SetWorldMatrix(result);

					return true;
				}

				return false;
			}

			DirectX::XMFLOAT2 WorldToScreen(const DirectX::XMFLOAT3& worldPos, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const ImVec2& viewportSize)
			{
				using namespace DirectX;

				XMVECTOR vWorld = XMLoadFloat3(&worldPos);
				XMVECTOR vClip = XMVector3Transform(vWorld, view * proj);

				// Perspective divide
				XMFLOAT4 clip;
				XMStoreFloat4(&clip, vClip);
				float ndcX = clip.x / clip.w;
				float ndcY = clip.y / clip.w;

				// Convert NDC to screen
				XMFLOAT2 screen;
				screen.x = (ndcX * 0.5f + 0.5f) * viewportSize.x;
				screen.y = (1.0f - (ndcY * 0.5f + 0.5f)) * viewportSize.y; // ImGui Y is top-left
				return screen;
			}

			void DrawDirectionalLightGizmo(const gallus::graphics::dx12::DirectionalLight& light, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const ImVec2& viewportSize)
			{
				ImDrawList* drawList = ImGui::GetForegroundDrawList();

				DirectX::XMFLOAT2 start = WorldToScreen(light.GetTransform().GetPosition(), view, proj, viewportSize);

				// End point = start + direction * length
				DirectX::XMFLOAT3 endWorld;
				endWorld.x = light.GetTransform().GetPosition().x + light.GetDirectionalLightData().LightDirection.x * 2.0f;
				endWorld.y = light.GetTransform().GetPosition().y + light.GetDirectionalLightData().LightDirection.y * 2.0f;
				endWorld.z = light.GetTransform().GetPosition().z + light.GetDirectionalLightData().LightDirection.z * 2.0f;

				DirectX::XMFLOAT2 end = WorldToScreen(endWorld, view, proj, viewportSize);

				drawList->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), IM_COL32(255, 255, 255, 255), 2.0f);
			}

			bool RenderObjectGizmos(const ImRect& a_SceneViewRect, ISerializableObject* a_pObject)
			{
				if (!a_pObject)
				{
					return false;
				}

				const auto& gizmos = a_pObject->GetEditorGizmos();

				bool changed = false;
				for (const EditorGizmoInfo& gizmo : gizmos)
				{
					void* ptr = reinterpret_cast<char*>(a_pObject) + gizmo.m_iOffset;

					switch (gizmo.m_Options.type)
					{
						case EditorGizmoType::EditorGizmoType_Transform:
						{
							graphics::dx12::Transform* value = reinterpret_cast<graphics::dx12::Transform*>(ptr);
							if (ShowTransformGizmo(a_SceneViewRect, *value))
							{
								changed = true;
							}
							break;
						}
						case EditorGizmoType::EditorGizmoType_Direction:
						{
							break;
						}
					}
				}

				return changed;
			}
		}
	}
}