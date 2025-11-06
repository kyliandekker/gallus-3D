#include "RenderEditorExposable.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_toggle.h>
#include <imgui/imgui_helpers.h>
#include <limits>

#include "utils/string_extensions.h"

#include "editor/core/EditorEngine.h"
#include "editor/EditorExpose.h"

#include "EditorWindowsConfig.h"

#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "gameplay/Prefab.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			bool ShowDragFloat(const std::string& a_sId, float* a_pValue, const EditorFieldInfo& a_Field)
			{
				float min = std::numeric_limits<float>::min();
				float max = std::numeric_limits<float>::max();
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
				float min = std::numeric_limits<float>::min();
				float max = std::numeric_limits<float>::max();
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

			void ShowObject(IExposableToEditor* obj)
			{
				if (!obj)
				{
					ImGui::TextDisabled("<null>");
					return;
				}

				for (const EditorFieldInfo& subField : obj->GetEditorFields())
				{
					ShowEditorFieldFromObject(obj, subField);
				}

			}

			void ShowAssetPicker(const std::string& a_sId, gallus::resources::EngineResource* a_pValue, gallus::resources::EngineResource** a_pValuePtr, const EditorFieldInfo& a_Field)
			{
				std::string name = (a_pValue != nullptr) ? a_pValue->GetName() : "<null>";

				char buf[256];
				strncpy_s(buf, sizeof(buf), name.c_str(), sizeof(buf) - 1);
				buf[sizeof(buf) - 1] = '\0';

				ImVec2 buttonSize = ImVec2(core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() * 2, core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() * 2);

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() / 2, core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() / 2));
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(a_sId.c_str(), buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, a_sId + BUTTON_ID, "").c_str(), buttonSize))
				{
					FilePickerModal& filePickerModal = core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetWindowsConfig<EditorWindowsConfig>().GetFilePickerModal();
					filePickerModal.SetData(
						[a_pValuePtr, a_pValue](int success, gallus::resources::FileResource& resource)
					{
						if (success == 1)
						{
							auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

							if (a_pValue->GetResourceType() == resources::AssetType::Sprite)
							{
								*a_pValuePtr = core::EDITOR_ENGINE->GetResourceAtlas()
									.LoadTexture(resource.GetPath().filename().generic_string(), cCommandQueue)
									.get();
							}
							else if (a_pValue->GetResourceType() == resources::AssetType::PixelShader)
							{
								*a_pValuePtr = core::EDITOR_ENGINE->GetResourceAtlas()
									.LoadPixelShader(resource.GetPath().filename().generic_string())
									.get();
							}
							else if (a_pValue->GetResourceType() == resources::AssetType::VertexShader)
							{
								*a_pValuePtr = core::EDITOR_ENGINE->GetResourceAtlas()
									.LoadVertexShader(resource.GetPath().filename().generic_string())
									.get();
							}
							else if (a_pValue->GetResourceType() == resources::AssetType::Prefab)
							{
								gameplay::Prefab& prefab = dynamic_cast<gameplay::Prefab&>(**a_pValuePtr);

								core::EDITOR_ENGINE->GetResourceAtlas().LoadPrefab(resource.GetPath().filename().generic_string(), prefab);
							}

							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					},
						std::vector<gallus::resources::AssetType>{ a_Field.m_Options.assetType });
					filePickerModal.Show();
				}
				ImGui::PopStyleVar();
			}

			void ShowEditorFieldFromObject(IExposableToEditor* obj, const EditorFieldInfo& field)
			{
				void* ptr = reinterpret_cast<char*>(obj) + field.m_iOffset;

				std::function<void()> func = [] {};

				bool showTable = true;
				std::string fieldId = ImGui::IMGUI_FORMAT_ID("", INPUT_ID, string_extensions::StringToUpper(field.m_sUIName) + "_INSPECTOR");
				switch (field.m_Options.type)
				{
					case EditorWidgetType::DragFloat:
					{
						float* value = reinterpret_cast<float*>(ptr);
						func = [value, &field, &fieldId]
						{
							ShowDragFloat(fieldId, value, field);
						};
						break;
					}
					case EditorWidgetType::DragInt8:
					{
						func = [ptr, &field, &fieldId]
						{
							int8_t* value = reinterpret_cast<int8_t*>(ptr);
							int temp = static_cast<int64_t>(*value);

							if (ShowDragInt(fieldId, temp, field))
							{
								*value = static_cast<int8_t>(temp);
							}
						};
						break;
					}

					case EditorWidgetType::DragInt16:
					{
						func = [ptr, &field, &fieldId]
						{
							int16_t* value = reinterpret_cast<int16_t*>(ptr);
							int temp = static_cast<int64_t>(*value);

							if (ShowDragInt(fieldId, temp, field))
							{
								*value = static_cast<int16_t>(temp);
							}
						};
						break;
					}

					case EditorWidgetType::DragInt32:
					{
						func = [ptr, &field, &fieldId]
						{
							int32_t* value = reinterpret_cast<int32_t*>(ptr);
							int temp = static_cast<int64_t>(*value);

							if (ShowDragInt(fieldId, temp, field))
							{
								*value = static_cast<int32_t>(temp);
							}
						};
						break;
					}

					case EditorWidgetType::DragInt64:
					{
						func = [ptr, &field, &fieldId]
						{
							int64_t* value = reinterpret_cast<int64_t*>(ptr);
							int temp = *value;

							if (ShowDragInt(fieldId, temp, field))
							{
								*value = temp;
							}
						};
						break;
					}
					case EditorWidgetType::Checkbox:
					{
						bool* value = reinterpret_cast<bool*>(ptr);
						func = [&field, &fieldId, value]
						{
							ImGui::Checkbox(fieldId.c_str(), value);
						};
						break;
					}
					case EditorWidgetType::Toggle:
					{
						bool* value = reinterpret_cast<bool*>(ptr);
						func = [&field, &fieldId, value]
						{
							ImGui::Toggle(fieldId.c_str(), value);
						};
						break;
					}
					case EditorWidgetType::Vector2Field:
					{
						DirectX::XMFLOAT2* value = reinterpret_cast<DirectX::XMFLOAT2*>(ptr);
						func = [&field, &fieldId, value]
						{
							ShowVector2(fieldId, *value, field);
						};
						break;
					}
					case EditorWidgetType::AssetPickerPtr:
					{
						gallus::resources::EngineResource** pValuePtr = reinterpret_cast<gallus::resources::EngineResource**>(ptr);
						
						func = [&field, &fieldId, pValuePtr, ptr]
						{
							gallus::resources::EngineResource* value = (pValuePtr && *pValuePtr) ? *pValuePtr : nullptr;
							ShowAssetPicker(fieldId, value, pValuePtr, field);
						};
						break;
					}
					case EditorWidgetType::AssetPicker:
					{
						gallus::resources::EngineResource* pValue = reinterpret_cast<gallus::resources::EngineResource*>(ptr);

						func = [pValue, &field, &fieldId]()
							{
								// Create a real pointer-to-pointer on the stack
								gallus::resources::EngineResource* pValueCopy = pValue;
								gallus::resources::EngineResource** ppValue = &pValueCopy;

								ShowAssetPicker(fieldId, pValue, ppValue, field);
							};
						break;
					}
					case EditorWidgetType::Object:
					{
						showTable = false;
						IExposableToEditor* editorObject = dynamic_cast<IExposableToEditor*>(reinterpret_cast<IExposableToEditor*>(ptr));

						ShowObject(editorObject);
						break;
					}
					case EditorWidgetType::ObjectPtr:
					{
						showTable = false;

						IExposableToEditor** ppEditorObject = reinterpret_cast<IExposableToEditor**>(ptr);
						IExposableToEditor* pEditorObject = (ppEditorObject ? *ppEditorObject : nullptr);

						ShowObject(pEditorObject);
						break;
					}
					default:
					{
						break;
					}
				}

				if (showTable)
				{
					ImGui::KeyValue([&field]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetBoldFont(), field.m_sUIName);
					}, func);
				}
			}

			void RenderEditorForObject(IExposableToEditor* obj)
			{
				const auto& fields = obj->GetEditorFields();

				std::string id = ImGui::IMGUI_FORMAT_ID("", TABLE_ID, string_extensions::StringToUpper(std::string(obj->GetTypeName())) + "_INSPECTOR");
				bool tableActive = ImGui::StartInspectorKeyVal(id, core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFramePadding());

				if (tableActive)
				{
					for (const EditorFieldInfo& field : fields)
					{
						ShowEditorFieldFromObject(obj, field);
					}
					ImGui::EndInspectorKeyVal(ImVec2());
				}
			}
}
	}
}