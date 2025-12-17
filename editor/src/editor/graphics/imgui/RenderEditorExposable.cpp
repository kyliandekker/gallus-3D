#include "RenderEditorExposable.h"

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

// resources
#include "resources/SrcData.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/EditorExpose.h"
#include "editor/graphics/imgui/EditorWindowsConfig.h"

// gameplay
#include "gameplay/Prefab.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			bool ShowDragFloat(const std::string& a_sId, float* a_pValue, const EditorFieldInfo& a_Field)
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
				return ImGui::DragFloat(a_sId.c_str(), a_pValue, 1, min, max);
			}

			bool ShowDragInt(const std::string& a_sId, int& a_Value, const EditorFieldInfo& a_Field)
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

				return ImGui::DragInt(a_sId.c_str(), &a_Value, 1.0f, min, max);
			}

			bool ShowVector2(const std::string& a_sId, DirectX::XMFLOAT2& a_vVector, const EditorFieldInfo& a_Field)
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

			bool ShowVector3(const std::string& a_sId, DirectX::XMFLOAT3& a_vVector, const EditorFieldInfo& a_Field)
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
				if (ImGui::VectorEdit3(a_sId.c_str(), val, 0.1f, min, max))
				{
					a_vVector.x = val[0];
					a_vVector.y = val[1];
					a_vVector.z = val[2];
					return true;
				}
				return false;
			}

			bool ShowQuaternion(const std::string& a_sId, DirectX::XMVECTOR& a_vVector, const EditorFieldInfo& a_Field)
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

			bool ShowObject(IExposableToEditor* obj, bool a_bInternal)
			{
				if (!obj)
				{
					ImGui::TextDisabled("<null>");
					return false;
				}

				bool changed = false;
				for (const EditorFieldInfo& subField : obj->GetEditorFields())
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
				const EditorFieldInfo& a_Field)
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
						[a_pWeak, &a_Field](int success, gallus::resources::FileResource& resource)
						{
							if (success == 1)
							{
								switch (a_Field.m_Options.assetType)
								{
									case resources::AssetType::Sprite:
									{
										auto cCommandQueue =
											core::EDITOR_ENGINE->GetDX12()
											.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

										*a_pWeak = core::EDITOR_ENGINE->GetResourceAtlas()
											.LoadTexture(resource.GetPath().filename().generic_string(), cCommandQueue);

										break;
									}
									case resources::AssetType::Mesh:
									{
										auto cCommandQueue =
											core::EDITOR_ENGINE->GetDX12()
											.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

										*a_pWeak = core::EDITOR_ENGINE->GetResourceAtlas()
											.LoadMesh(resource.GetPath().filename().generic_string(), cCommandQueue);

										break;
									}
									case resources::AssetType::PixelShader:
									{
										*a_pWeak = core::EDITOR_ENGINE->GetResourceAtlas()
											.LoadPixelShader(resource.GetPath().filename().generic_string());
										break;
									}
									case resources::AssetType::VertexShader:
									{
										*a_pWeak = core::EDITOR_ENGINE->GetResourceAtlas()
											.LoadVertexShader(resource.GetPath().filename().generic_string());
										break;
									}
									case resources::AssetType::Prefab:
									{
										*a_pWeak = core::EDITOR_ENGINE->GetResourceAtlas()
											.LoadPrefab(resource.GetPath().filename().generic_string());
										break;
									}
								}
							}
						},
						std::vector<gallus::resources::AssetType>{ a_Field.m_Options.assetType });

					filePickerModal.Show();
				}

				ImGui::ShowTooltip(a_Field.m_Options.description);
				ImGui::PopStyleVar();

				return changed;
			}

			bool ShowEnumDropdown(const std::string& a_sId, int* a_pValue, const EditorFieldInfo& a_Field)
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
				ImGui::ShowTooltip(a_Field.m_Options.description);

				return changed;
			}

			bool ShowTexturePreview(const std::string& a_sId, graphics::dx12::Texture* a_pTexture, int spriteIndex)
			{
				if (!a_pTexture)
				{
					ImGui::TextDisabled("<no texture>");
					return false;
				}

				// Get the sprite UVs from the texture atlas
				ImVec2 size = ImVec2(100, 100); // preview size
				const ImVec2 uv0 = ImVec2(a_pTexture->GetSpriteUV(spriteIndex).uv0.x, a_pTexture->GetSpriteUV(spriteIndex).uv0.y);  // top-left UV
				const ImVec2 uv1 = ImVec2(a_pTexture->GetSpriteUV(spriteIndex).uv1.x, a_pTexture->GetSpriteUV(spriteIndex).uv1.y); // bottom-right UV

				ImGui::Image(a_pTexture->GetGPUHandle().ptr, size, uv0, uv1);

				return false; // read-only
			}

			bool ShowEditorFieldFromObject(IExposableToEditor* a_pObject, const EditorFieldInfo& a_Field, bool a_bInternal)
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
					case EditorFieldWidgetType::DragFloat:
					{
						float* value = reinterpret_cast<float*>(ptr);
						func = [value, &a_Field, &fieldId]
						{
							bool val = ShowDragFloat(fieldId, value, a_Field);
							ImGui::ShowTooltip(a_Field.m_Options.description);
							return val;
						};
						break;
					}
					case EditorFieldWidgetType::DragInt8:
					{
						func = [ptr, &a_Field, &fieldId]
						{
							int8_t* value = reinterpret_cast<int8_t*>(ptr);
							int temp = static_cast<int64_t>(*value);

							bool changed = ShowDragInt(fieldId, temp, a_Field);
							ImGui::ShowTooltip(a_Field.m_Options.description);
							if (changed)
							{
								*value = static_cast<int8_t>(temp);
							}
							return changed;
						};
						break;
					}
					case EditorFieldWidgetType::DragInt16:
					{
						func = [ptr, &a_Field, &fieldId]
						{
							int16_t* value = reinterpret_cast<int16_t*>(ptr);
							int temp = static_cast<int64_t>(*value);

							bool changed = ShowDragInt(fieldId, temp, a_Field);
							ImGui::ShowTooltip(a_Field.m_Options.description);
							if (changed)
							{
								*value = static_cast<int16_t>(temp);
							}
							return changed;
						};
						break;
					}
					case EditorFieldWidgetType::DragInt32:
					{
						func = [ptr, &a_Field, &fieldId]
						{
							int32_t* value = reinterpret_cast<int32_t*>(ptr);
							int temp = static_cast<int64_t>(*value);

							bool changed = ShowDragInt(fieldId, temp, a_Field);
							ImGui::ShowTooltip(a_Field.m_Options.description);
							if (changed)
							{
								*value = static_cast<int32_t>(temp);
							}
							return changed;
						};
						break;
					}
					case EditorFieldWidgetType::DragInt64:
					{
						func = [ptr, &a_Field, &fieldId]
						{
							int64_t* value = reinterpret_cast<int64_t*>(ptr);
							int temp = *value;

							bool changed = ShowDragInt(fieldId, temp, a_Field);
							ImGui::ShowTooltip(a_Field.m_Options.description);
							if (changed)
							{
								*value = temp;
							}
							return changed;
						};
						break;
					}
					case EditorFieldWidgetType::Checkbox:
					{
						bool* value = reinterpret_cast<bool*>(ptr);
						func = [&a_Field, &fieldId, value]
						{
							bool val = ImGui::Checkbox(fieldId.c_str(), value);
							ImGui::ShowTooltip(a_Field.m_Options.description);
							return val;
						};
						break;
					}
					case EditorFieldWidgetType::Toggle:
					{
						bool* value = reinterpret_cast<bool*>(ptr);
						func = [&a_Field, &fieldId, value]
						{
							bool val = ImGui::Toggle(fieldId.c_str(), value);
							ImGui::ShowTooltip(a_Field.m_Options.description);
							return val;
						};
						break;
					}
					case EditorFieldWidgetType::Vector2Field:
					{
						DirectX::XMFLOAT2* value = reinterpret_cast<DirectX::XMFLOAT2*>(ptr);
						func = [&a_Field, &fieldId, value]
						{
							bool val = ShowVector2(fieldId, *value, a_Field);
							ImGui::ShowTooltip(a_Field.m_Options.description);
							return val;
						};
						break;
					}
					case EditorFieldWidgetType::Vector3Field:
					{
						DirectX::XMFLOAT3* value = reinterpret_cast<DirectX::XMFLOAT3*>(ptr);
						func = [&a_Field, &fieldId, value]
						{
							bool val = ShowVector3(fieldId, *value, a_Field);
							ImGui::ShowTooltip(a_Field.m_Options.description);
							return val;
						};
						break;
					}
					case EditorFieldWidgetType::Quaternion:
					{
						DirectX::XMVECTOR* value = reinterpret_cast<DirectX::XMVECTOR*>(ptr);

						func = [&a_Field, &fieldId, value]
						{
							bool val = ShowQuaternion(fieldId, *value, a_Field);
							ImGui::ShowTooltip(a_Field.m_Options.description);
							return val;
						};
						break;
					}
					case EditorFieldWidgetType::AssetPickerPtr:
					{
						std::weak_ptr<gallus::resources::EngineResource>* pWeak =
							reinterpret_cast<std::weak_ptr<gallus::resources::EngineResource>*>(ptr);

						func = [&a_Field, &fieldId, pWeak]()
						{
							std::shared_ptr<gallus::resources::EngineResource> locked;
							if (pWeak)
							{
								locked = pWeak->lock();
							}

							gallus::resources::EngineResource* pLocked =
								(locked.get()) ? locked.get() : nullptr;

							return ShowAssetPicker(fieldId, pLocked, pWeak, a_Field);
						};

						break;
					}
					case EditorFieldWidgetType::Object:
					{
						showTable = false;
						IExposableToEditor* editorObject = dynamic_cast<IExposableToEditor*>(reinterpret_cast<IExposableToEditor*>(ptr));

						return ShowObject(editorObject, a_bInternal);
						break;
					}
					case EditorFieldWidgetType::ObjectPtr:
					{
						showTable = false;

						IExposableToEditor** ppEditorObject = reinterpret_cast<IExposableToEditor**>(ptr);
						IExposableToEditor* pEditorObject = (ppEditorObject ? *ppEditorObject : nullptr);

						return ShowObject(pEditorObject, a_bInternal);
						break;
					}
					case EditorFieldWidgetType::EnumDropdown:
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
					case EditorFieldWidgetType::TexturePreview:
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
						ImGui::ShowTooltip(a_Field.m_Options.description);
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

			bool RenderObjectFields(IExposableToEditor* a_pObject)
			{
				const auto& fields = a_pObject->GetEditorFields();

				std::string id = ImGui::IMGUI_FORMAT_ID("", TABLE_ID, string_extensions::StringToUpper(std::string(a_pObject->GetTypeName())) + "_INSPECTOR");
				bool tableActive = ImGui::StartInspectorKeyVal(id, core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFramePadding());

				bool changed = false;
				if (tableActive)
				{
					for (const EditorFieldInfo& field : fields)
					{
						if (ShowEditorFieldFromObject(a_pObject, field, field.m_Options.internal))
						{
							changed = true;
						}
					}
					ImGui::EndInspectorKeyVal(ImVec2());
				}

				return changed;
			}

			bool ShowTransformGizmo(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom, graphics::dx12::Transform& a_Transform)
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

				if (ImGui::IsKeyPressed(ImGuiKey_T) || ImGui::IsKeyPressed(ImGuiKey_P))
				{
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::TRANSLATE);
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				}
				if (ImGui::IsKeyPressed(ImGuiKey_R))
				{
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::ROTATE);
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				}
				if (ImGui::IsKeyPressed(ImGuiKey_S))
				{
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::SCALE);
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				}

				bool useSnap = ImGui::IsKeyDown(ImGuiKey_LeftShift);
				float snap = useSnap ? 1.0f : 0.0f;

				if (ImGuizmo::Manipulate(
					viewFloat,
					projFloat,
					(ImGuizmo::OPERATION) core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation(),
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

			bool RenderObjectGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom, IExposableToEditor* a_pObject)
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
						case EditorGizmoType::Transform:
						{
							graphics::dx12::Transform* value = reinterpret_cast<graphics::dx12::Transform*>(ptr);
							if (ShowTransformGizmo(a_vScenePos, a_vSize, a_vPanOffset, a_fZoom, *value))
							{
								changed = true;
							}
							break;
						}
					}
				}

				return changed;
			}
		}
	}
}